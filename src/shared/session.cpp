/*
* Copyright (C) 2008-2013 The Communi Project
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
#include <IrcCommand>
#include <IrcMessage>
#include <Irc>
#ifndef QT_NO_OPENSSL
#include <QSslSocket>
#endif // QT_NO_OPENSSL
#include <QNetworkConfigurationManager>

QNetworkSession* Session::s_network = 0;

Session::Session(QObject* parent) : IrcSession(parent),
    m_info(this), m_quit(false), m_timestamp(0), m_meter(new IrcLagMeter(this))
{
    installMessageFilter(this);

    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(this, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
    connect(this, SIGNAL(capabilities(QStringList, QStringList*)), this, SLOT(onCapabilities(QStringList, QStringList*)));
    connect(this, SIGNAL(sessionInfoReceived(IrcSessionInfo)), SLOT(onSessionInfoReceived(IrcSessionInfo)));

    setAutoReconnectDelay(15);
    connect(&m_reconnectTimer, SIGNAL(timeout()), this, SLOT(open()));

    m_timestamper.invalidate();
}

Session::~Session()
{
}

QString Session::name() const
{
    return m_name;
}

void Session::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
    }
}

QString Session::network() const
{
    return IrcSessionInfo(this).network();
}

IrcLagMeter* Session::lagMeter() const
{
    return m_meter;
}

int Session::autoReconnectDelay() const
{
    return m_reconnectTimer.interval() / 1000;
}

void Session::setAutoReconnectDelay(int delay)
{
    m_reconnectTimer.setInterval(delay * 1000);
}

ChannelInfos Session::channels() const
{
    return m_channels;
}

void Session::addChannel(const QString& channel)
{
    const QString lower = channel.toLower();
    foreach (const ChannelInfo& info, m_channels) {
        if (info.channel.toLower() == lower)
            return;
    }

    ChannelInfo info;
    info.channel = channel;
    m_channels.append(info);
}

void Session::setChannelKey(const QString& channel, const QString& key)
{
    int idx = -1;
    const QString lower = channel.toLower();
    for (int i = 0; idx == -1 && i < m_channels.count(); ++i)
        if (m_channels.at(i).channel.toLower() == lower)
            idx = i;
    if (idx != -1) {
        ChannelInfo info = m_channels.at(idx);
        info.key = key;
        m_channels.replace(idx, info);
    }
}

void Session::removeChannel(const QString& channel)
{
    int idx = -1;
    const QString lower = channel.toLower();
    for (int i = 0; idx == -1 && i < m_channels.count(); ++i)
        if (m_channels.at(i).channel.toLower() == lower)
            idx = i;
    if (idx != -1)
        m_channels.removeAt(idx);
}

void Session::setChannels(const ChannelInfos& channels)
{
    m_channels = channels;
}

bool Session::isChannel(const QString& receiver) const
{
    return receiver.length() > 1 && m_info.channelTypes().contains(receiver.at(0));
}

QString Session::userPrefix(const QString& user) const
{
    QStringList prefixes = m_info.prefixes();
    int i = 0;
    while (i < user.length() && prefixes.contains(user.at(i)))
        ++i;
    return user.left(i);
}

QString Session::unprefixedUser(const QString& user) const
{
    QStringList prefixes = IrcSessionInfo(this).prefixes();
    QString copy = user;
    while (!copy.isEmpty() && prefixes.contains(copy.at(0)))
        copy.remove(0, 1);
    IrcSender sender(copy);
    if (sender.isValid())
        copy = sender.name();
    return copy;
}

bool Session::isSecure() const
{
#ifdef QT_NO_OPENSSL
    return false;
#else
    return qobject_cast<QSslSocket*>(socket());
#endif // QT_NO_OPENSSL
}

void Session::setSecure(bool secure)
{
#ifdef QT_NO_OPENSSL
    Q_UNUSED(secure)
#else
    QSslSocket* sslSocket = qobject_cast<QSslSocket*>(socket());
    if (secure && !sslSocket) {
        sslSocket = new QSslSocket(this);
        sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
        sslSocket->ignoreSslErrors();
        setSocket(sslSocket);
    } else if (!secure && sslSocket) {
        setSocket(new QTcpSocket(this));
    }
#endif // QT_NO_OPENSSL
}

QString Session::password() const
{
    return m_password;
}

void Session::setPassword(const QString& password)
{
    m_password = password;
}

ConnectionInfo Session::toConnection() const
{
    ConnectionInfo connection;
    connection.name = name();
    connection.secure = isSecure();
    connection.host = host();
    connection.port = port();
    connection.user = userName();
    connection.nick = nickName();
    connection.real = realName();
    connection.pass = password();
    connection.channels = channels();
    connection.quit = m_quit;
    return connection;
}

void Session::initFrom(const ConnectionInfo& connection)
{
    setName(connection.name);
    setSecure(connection.secure);
    setPassword(connection.pass);
    setHost(connection.host);
    setPort(connection.port);
    setNickName(connection.nick);
    QString appName = QApplication::applicationName().toLower();
    setUserName(connection.user.isEmpty() ? appName : connection.user);
    setRealName(connection.real.isEmpty() ? appName : connection.real);
    setChannels(connection.channels);
    m_quit = connection.quit;
}

Session* Session::fromConnection(const ConnectionInfo& connection, QObject* parent)
{
    Session* session = new Session(parent);
    session->initFrom(connection);
    return session;
}

bool Session::hasQuit() const
{
    return m_quit;
}

bool Session::isReconnecting() const
{
    return m_reconnectTimer.isActive();
}

bool Session::ensureNetwork()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        if (!s_network)
            s_network = new QNetworkSession(manager.defaultConfiguration(), qApp);
        s_network->open();
    }
    // TODO: return value?
    return true;
}

bool Session::sendUiCommand(IrcCommand* command)
{
    if (command->type() == IrcCommand::Join) {
        QString key = command->parameters().value(1);
        if (!key.isEmpty())
            setChannelKey(command->parameters().value(0), key);
    }
    return sendCommand(command);
}

void Session::reconnect()
{
    connect(this, SIGNAL(connecting()), &m_reconnectTimer, SLOT(stop()), Qt::UniqueConnection);
    connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), &m_reconnectTimer, SLOT(start()), Qt::UniqueConnection);

    if (ensureNetwork())
        open();
}

void Session::quit()
{
    sleep();
    m_quit = true;
}

void Session::destructLater()
{
    if (isConnected()) {
        connect(this, SIGNAL(disconnected()), SLOT(deleteLater()));
        connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), SLOT(deleteLater()));
        QTimer::singleShot(1000, this, SLOT(deleteLater()));
    } else {
        deleteLater();
    }
}

void Session::stopReconnecting()
{
    m_reconnectTimer.stop();
}

void Session::sleep()
{
    disconnect(this, SIGNAL(connecting()), &m_reconnectTimer, SLOT(stop()));
    disconnect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), &m_reconnectTimer, SLOT(start()));

    QString message = tr("%1 %2").arg(QApplication::applicationName())
                                 .arg(QApplication::applicationVersion());

    if (isConnected()) {
        if (m_info.activeCapabilities().contains("communi"))
            sendCommand(IrcCommand::createCtcpRequest("*communi", "TIME"));
        sendCommand(IrcCommand::createQuit(message));
    } else {
        close();
    }
}

void Session::wake()
{
    if (!m_quit)
        reconnect();
}

void Session::onConnected()
{
    foreach (const ChannelInfo& channel, m_channels) {
        if (!channel.channel.isEmpty())
            sendCommand(IrcCommand::createJoin(channel.channel, channel.key));
    }
    m_quit = false;

    m_timestamper.invalidate();
}

void Session::onDisconnected()
{
}

void Session::onPassword(QString* password)
{
    *password = m_password;
}

void Session::onCapabilities(const QStringList& available, QStringList* request)
{
    if (available.contains("identify-msg"))
        request->append("identify-msg");
    if (available.contains("communi")) {
        request->append("communi");
        request->append(QString("communi/%1").arg(m_timestamp));
    }
}

void Session::onSessionInfoReceived(const IrcSessionInfo& info)
{
    m_info = info;
    emit networkChanged(m_info.network());
}

bool Session::messageFilter(IrcMessage* message)
{
    if (m_timestamp > 0 && m_timestamper.isValid()) {
        long elapsed = m_timestamper.elapsed() / 1000;
        if (elapsed > 0) {
            m_timestamp += elapsed;
            m_timestamper.restart();
        }
    }

    if (message->type() == IrcMessage::Join) {
        if (message->flags() & IrcMessage::Own)
            addChannel(static_cast<IrcJoinMessage*>(message)->channel());
    } else if (message->type() == IrcMessage::Part) {
        if (message->flags() & IrcMessage::Own)
            removeChannel(static_cast<IrcPartMessage*>(message)->channel());
    } else if (message->type() == IrcMessage::Numeric) {
        int code = static_cast<IrcNumericMessage*>(message)->code();
        if (code == Irc::ERR_NICKNAMEINUSE) {
            if (m_alternateNicks.isEmpty()) {
                QString currentNick = nickName();
                m_alternateNicks << (currentNick + "_")
                                 <<  currentNick
                                 << (currentNick + "__")
                                 <<  currentNick;
            }
            setNickName(m_alternateNicks.takeFirst());
        }
    } else if (message->type() == IrcMessage::Notice) {
        if (message->sender().name() == "*communi") {
            m_timestamp = static_cast<IrcNoticeMessage*>(message)->message().toLong();
            m_timestamper.restart();
            return true;
        }
    }
    return false;
}
