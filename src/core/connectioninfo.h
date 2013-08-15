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

#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QList>
#include <QString>
#include <QVariant>
#include <QMetaType>
#include <QDataStream>
#include "viewinfo.h"
#include "streamer.h"

class Connection;

struct ConnectionInfo
{
    ConnectionInfo();

    Connection* toConnection(QObject* parent = 0) const;
    void initConnection(Connection* connection) const;
    static ConnectionInfo fromConnection(Connection* connection);

    QString name;
    QString host;
    quint16 port;
    QString nick;
    QString real;
    QString pass;
    bool secure;
    ViewInfos views;
    QString user;
    bool quit;
};
Q_DECLARE_METATYPE(ConnectionInfo);

inline QDataStream& operator<<(QDataStream& out, const ConnectionInfo& connection)
{
    out << quint32(123); // version
    out << connection.name;
    out << connection.host;
    out << connection.port;
    out << connection.nick;
    out << connection.real;
    out << connection.pass;
    out << connection.secure;
    out << connection.views;
    out << connection.user;
    out << connection.quit;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, ConnectionInfo& connection)
{
    quint32 version = readStreamValue<quint32>(in, 0);
    connection.name = readStreamValue<QString>(in, connection.name);
    connection.host = readStreamValue<QString>(in, connection.host);
    connection.port = readStreamValue<quint16>(in, connection.port);
    connection.nick = readStreamValue<QString>(in, connection.nick);
    connection.real = readStreamValue<QString>(in, connection.real);
    connection.pass = readStreamValue<QString>(in, connection.pass);
    connection.secure = readStreamValue<bool>(in, connection.secure);
    connection.views = readStreamValue<ViewInfos>(in, connection.views);
    connection.user = readStreamValue<QString>(in, connection.user);
    connection.quit = readStreamValue<bool>(in, connection.quit);
    Q_UNUSED(version);
    return in;
}

typedef QList<ConnectionInfo> ConnectionInfos;
Q_DECLARE_METATYPE(ConnectionInfos);

#endif // CONNECTIONINFO_H
