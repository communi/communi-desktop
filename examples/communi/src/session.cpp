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
#include "application.h"
#ifndef QT_NO_OPENSSL
#include <QSslSocket>
#endif // QT_NO_OPENSSL
#include <ircmessage.h>
#include <irccommand.h>

Session::Session(QObject* parent) : IrcSession(parent)
{
    connect(this, SIGNAL(password(QString*)), SLOT(onPassword(QString*)));
    Application::setSessions(Application::sessions() << this);
}

Session::~Session()
{
    QList<Session*> sessions = Application::sessions();
    if (sessions.removeOne(this))
        Application::setSessions(sessions);
}

Connection Session::connection() const
{
    return conn;
}

void Session::connectTo(const Connection& connection)
{
#ifndef QT_NO_OPENSSL
    if (connection.secure)
    {
        QSslSocket* socket = new QSslSocket(this);
        socket->setPeerVerifyMode(QSslSocket::VerifyNone);
        socket->ignoreSslErrors();
        setSocket(socket);
    }
#endif // QT_NO_OPENSSL
    
    QString appName = QApplication::applicationName();

    setHost(connection.host);
    setPort(connection.port);
    setNickName(connection.nick);
    setUserName(appName.toLower());
    setRealName(connection.real.isEmpty() ? appName : connection.real);
    open();

    conn = connection;
}

void Session::onPassword(QString* password)
{
    *password = conn.pass;
}
