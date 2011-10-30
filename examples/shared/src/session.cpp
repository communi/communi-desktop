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

Session::Session(QObject *parent) : IrcSession(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
    connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), &m_timer, SLOT(start()));
    connect(this, SIGNAL(connecting()), &m_timer, SLOT(stop()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(open()));
    setAutoReconnectDelay(15);
}

QString Session::name() const
{
    return m_name;
}

void Session::setName(const QString& name)
{
    m_name = name;
}

int Session::autoReconnectDelay() const
{
    return m_timer.interval();
}

void Session::setAutoReconnectDelay(int delay)
{
    m_timer.setInterval(delay * 1000);
}

QStringList Session::autoJoinChannels() const
{
    return m_channels;
}

void Session::setAutoJoinChannels(const QStringList& channels)
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
    connection.nick = nickName();
    connection.real = realName();
    connection.pass = password();
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
    session->setUserName(appName.toLower());
    session->setRealName(connection.real.isEmpty() ? appName : connection.real);
    session->setAutoJoinChannels(connection.channels);
    return session;
}

void Session::onConnected()
{
    foreach (const QString& channel, m_channels)
        sendCommand(IrcCommand::createJoin(channel, QString()));
}

void Session::onPassword(QString* password)
{
    *password = m_password;
}
