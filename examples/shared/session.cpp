/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "session.h"
#include <QApplication>
#include <QSslSocket>
#include <IrcCommand>
#include <IrcMessage>
#include <QNetworkConfigurationManager>
#include <Irc>

QNetworkSession* Session::s_network = 0;

Session::Session(QObject *parent) : IrcSession(parent),
    m_currentLag(-1), m_maxLag(120000)
{
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
    connect(this, SIGNAL(messageReceived(IrcMessage*)), SLOT(handleMessage(IrcMessage*)));

    setAutoReconnectDelay(15);
    connect(&m_reconnectTimer, SIGNAL(timeout()), this, SLOT(open()));

    connect(&m_pingTimer, SIGNAL(timeout()), this, SLOT(pingServer()));
    connect(this, SIGNAL(connectedChanged(bool)), SLOT(togglePingTimer(bool)));
}

QString Session::name() const
{
    return m_name;
}

void Session::setName(const QString& name)
{
    if (m_name != name)
    {
        m_name = name;
        emit nameChanged(name);
    }
}

QString Session::network() const
{
    return m_info.value("NETWORK");
}

int Session::autoReconnectDelay() const
{
    return m_reconnectTimer.interval() / 1000;
}

void Session::setAutoReconnectDelay(int delay)
{
    m_reconnectTimer.setInterval(delay * 1000);
}

QStringList Session::channels() const
{
    return m_channels;
}

void Session::setChannels(const QStringList& channels)
{
    m_channels = channels;
}

bool Session::isSecure() const
{
    return qobject_cast<QSslSocket*>(socket());
}

void Session::setSecure(bool secure)
{
    QSslSocket* sslSocket = qobject_cast<QSslSocket*>(socket());
    if (secure && !sslSocket)
    {
        sslSocket = new QSslSocket(this);
        sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
        sslSocket->ignoreSslErrors();
        setSocket(sslSocket);
    }
    else if (!secure && sslSocket)
    {
        setSocket(new QTcpSocket(this));
    }
}

QString Session::password() const
{
    return m_password;
}

void Session::setPassword(const QString& password)
{
    m_password = password;
}

Connection Session::toConnection() const
{
    Connection connection;
    connection.name = name();
    connection.secure = isSecure();
    connection.host = host();
    connection.port = port();
    connection.user = userName();
    connection.nick = nickName();
    connection.real = realName();
    connection.pass = password();
    connection.channels = channels();
    return connection;
}

Session* Session::fromConnection(const Connection& connection, QObject* parent)
{
    Session* session = new Session(parent);
    session->setName(connection.name);
    session->setSecure(connection.secure);
    session->setPassword(connection.pass);
    QString appName = QApplication::applicationName();
    session->setHost(connection.host);
    session->setPort(connection.port);
    session->setNickName(connection.nick);
    session->setUserName(connection.user.isEmpty() ? appName : connection.user);
    session->setRealName(connection.real.isEmpty() ? appName : connection.real);
    session->setChannels(connection.channels);
    return session;
}

int Session::pingInterval() const
{
    return m_pingTimer.interval() / 1000;
}

void Session::setPingInterval(int interval)
{
    m_pingTimer.start(interval * 1000);
}

int Session::currentLag() const
{
    return m_currentLag;
}

int Session::maximumLag() const
{
    return m_maxLag;
}

void Session::setMaximumLag(int lag)
{
    m_maxLag = lag;
}

bool Session::ensureNetwork()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        if (!s_network)
            s_network = new QNetworkSession(manager.defaultConfiguration(), qApp);
        s_network->open();
    }
    // TODO: return value?
    return true;
}

void Session::reconnect()
{
    connect(this, SIGNAL(connecting()), &m_reconnectTimer, SLOT(stop()));
    connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), &m_reconnectTimer, SLOT(start()));

    if (ensureNetwork())
        open();
}

void Session::quit(const QString& reason)
{
    disconnect(this, SIGNAL(connecting()), &m_reconnectTimer, SLOT(stop()));
    disconnect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), &m_reconnectTimer, SLOT(start()));

    QString message = reason;
    if (message.isEmpty())
        message = tr("%1 %2").arg(QApplication::applicationName())
                             .arg(QApplication::applicationVersion());

    if (isConnected())
        sendCommand(IrcCommand::createQuit(message));
    socket()->waitForDisconnected(1000);
    close();
}

void Session::destructLater()
{
    if (isConnected())
    {
        connect(this, SIGNAL(disconnected()), SLOT(deleteLater()));
        connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), SLOT(deleteLater()));
        QTimer::singleShot(1000, this, SLOT(deleteLater()));
    }
    else
    {
        deleteLater();
    }
}

void Session::onConnected()
{
    foreach (const QString& channel, m_channels)
    {
        if (!channel.isEmpty())
            sendCommand(IrcCommand::createJoin(channel, QString()));
    }
}

void Session::onPassword(QString* password)
{
    *password = m_password;
}

void Session::handleMessage(IrcMessage* message)
{
    // 20s delay since the last message was received
    setPingInterval(20);

    if (message->type() == IrcMessage::Join)
    {
        if (message->sender().name() == nickName())
        {
            QString channel = static_cast<IrcJoinMessage*>(message)->channel();
            int idx = m_channels.indexOf(QRegExp(channel, Qt::CaseInsensitive));
            if (idx == -1)
                m_channels.append(channel);
        }
    }
    else if (message->type() == IrcMessage::Part)
    {
        if (message->sender().name() == nickName())
        {
            QString channel = static_cast<IrcPartMessage*>(message)->channel();
            int idx = m_channels.indexOf(QRegExp(channel, Qt::CaseInsensitive));
            if (idx != -1)
                m_channels.removeAt(idx);
        }
    }
    else if (message->type() == IrcMessage::Pong)
    {
        if (message->parameters().contains("_C_o_m_m_u_n_i_"))
        {
            // slow down to 60s intervals
            setPingInterval(60);

            updateLag(static_cast<int>(m_lagTimer.elapsed()));
            m_lagTimer.invalidate();
        }
    }
    else if (message->type() == IrcMessage::Numeric)
    {
        if (static_cast<IrcNumericMessage*>(message)->code() == Irc::RPL_ISUPPORT)
        {
            foreach (const QString& param, message->parameters().mid(1))
            {
                QStringList keyValue = param.split("=", QString::SkipEmptyParts);
                m_info.insert(keyValue.value(0), keyValue.value(1));
            }
            if (m_info.contains("NETWORK"))
                emit networkChanged(m_info.value("NETWORK"));
        }
    }
}

void Session::pingServer()
{
    if (m_lagTimer.isValid())
    {
        // still lagging (no response since last PING)
        updateLag(static_cast<int>(m_lagTimer.elapsed()));

        // decrease the interval (60s => 20s => 6s => 2s)
        int interval = pingInterval();
        if (interval >= 6)
            setPingInterval(interval / 3);
    }
    else
    {
        // (re-)PING!
        m_lagTimer.start();
        sendRaw("PING _C_o_m_m_u_n_i_");
    }
}

void Session::updateLag(int lag)
{
    if (m_currentLag != lag)
    {
        m_currentLag = lag;
        emit currentLagChanged(lag);

        // TODO: https://github.com/communi/communi/issues/6
//        if (lag > m_maxLag)
//        {
//            close();
//            if (ensureNetwork())
//                open();
//        }
    }
}

void Session::togglePingTimer(bool enabled)
{
    if (enabled)
    {
        m_lagTimer.invalidate();
        m_pingTimer.start();
        pingServer();
    }
    else
    {
        m_pingTimer.stop();
        updateLag(-1);
    }
}
