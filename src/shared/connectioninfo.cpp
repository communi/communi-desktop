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

#include "connectioninfo.h"
#include "session.h"
#include <QCoreApplication>

ConnectionInfo::ConnectionInfo() : port(6667), secure(false), quit(false)
{
}

Session* ConnectionInfo::toSession(QObject* parent) const
{
    Session* session = new Session(parent);
    initSession(session);
    return session;
}

void ConnectionInfo::initSession(Session* session) const
{
    session->setName(name);
    session->setSecure(secure);
    session->setPassword(pass);
    session->setHost(host);
    session->setPort(port);
    session->setNickName(nick);
    QString appName = QCoreApplication::applicationName().toLower();
    session->setUserName(user.isEmpty() ? appName : user);
    session->setRealName(real.isEmpty() ? appName : real);
    session->setViews(views);
    session->setHasQuit(quit);
}

ConnectionInfo ConnectionInfo::fromSession(Session* session)
{
    ConnectionInfo connection;
    connection.name = session->name();
    connection.secure = session->isSecure();
    connection.host = session->host();
    connection.port = session->port();
    connection.user = session->userName();
    connection.nick = session->nickName();
    connection.real = session->realName();
    connection.pass = session->password();
    connection.views = session->views();
    connection.quit = session->hasQuit();
    return connection;
}
