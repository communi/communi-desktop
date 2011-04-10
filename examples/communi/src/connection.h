/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QList>
#include <QString>
#include <QVariant>
#include <QMetaType>
#include <QDataStream>
#include <QStringList>
#include "streamer.h"

struct Connection
{
    Connection();
    operator QVariant() const;
    bool operator==(const Connection& other) const;
    bool operator!=(const Connection& other) const;

    QString name;
    QString host;
    quint16 port;
    QString nick;
    QString real;
    QString pass;
    bool secure;
    QStringList channels;
};
Q_DECLARE_METATYPE(Connection);

inline QDataStream& operator<<(QDataStream& out, const Connection& connection)
{
    out << connection.name;
    out << connection.host;
    out << connection.port;
    out << connection.nick;
    out << connection.real;
    out << connection.pass;
    out << connection.secure;
    out << connection.channels;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, Connection& connection)
{
    connection.name = readStreamValue<QString>(in, connection.name);
    connection.host = readStreamValue<QString>(in, connection.host);
    connection.port = readStreamValue<quint16>(in, connection.port);
    connection.nick = readStreamValue<QString>(in, connection.nick);
    connection.real = readStreamValue<QString>(in, connection.real);
    connection.pass = readStreamValue<QString>(in, connection.pass);
    connection.secure = readStreamValue<bool>(in, connection.secure);
    connection.channels = readStreamValue<QStringList>(in, connection.channels);
    return in;
}

typedef QList<Connection> Connections;
Q_DECLARE_METATYPE(Connections);

#endif // CONNECTION_H
