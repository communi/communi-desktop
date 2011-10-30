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
#include <QSslSocket>

Session::Session(QObject *parent) : IrcSession(parent)
{
    connect(this, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
    connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), &m_timer, SLOT(start()));
    connect(this, SIGNAL(connecting()), &m_timer, SLOT(stop()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(open()));
    setAutoReconnectDelay(15);
}

int Session::autoReconnectDelay() const
{
    return m_timer.interval();
}

void Session::setAutoReconnectDelay(int delay)
{
    m_timer.setInterval(delay * 1000);
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
        setSocket(sslSocket);
    }
    else if (!secure && sslSocket)
    {
        setSocket(new QTcpSocket(this));
    }
}

QString Session::getPassword() const
{
    return m_password;
}

void Session::setPassword(const QString& password)
{
    m_password = password;
}

void Session::onPassword(QString* password)
{
    *password = m_password;
}
