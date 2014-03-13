/*
* Copyright (C) 2008-2014 The Communi Project
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
#include "connection.h"
#include <QCoreApplication>

ConnectionInfo::ConnectionInfo() : port(6667), secure(false), quit(false)
{
}

Connection* ConnectionInfo::toConnection(QObject* parent) const
{
    Connection* connection = new Connection(parent);
    initConnection(connection);
    return connection;
}

void ConnectionInfo::initConnection(Connection* connection) const
{
    connection->setDisplayName(name);
    connection->setSecure(secure);
    connection->setPassword(pass);
    connection->setHost(host);
    connection->setPort(port);
    connection->setNickName(nick);
    QString appName = QCoreApplication::applicationName().toLower();
    connection->setUserName(user.isEmpty() ? appName : user);
    connection->setRealName(real.isEmpty() ? appName : real);
    connection->setViews(views);
    if (quit)
        connection->setEnabled(false);
}

ConnectionInfo ConnectionInfo::fromConnection(Connection* connection)
{
    ConnectionInfo info;
    info.name = connection->displayName();
    info.secure = connection->isSecure();
    info.host = connection->host();
    info.port = connection->port();
    info.user = connection->userName();
    info.nick = connection->nickName();
    info.real = connection->realName();
    info.pass = connection->password();
    info.views = connection->views();
    info.quit = !connection->isEnabled();
    return info;
}
