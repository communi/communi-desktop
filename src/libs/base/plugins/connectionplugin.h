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

#ifndef CONNECTIONPLUGIN_H
#define CONNECTIONPLUGIN_H

#include <QList>
#include <QtPlugin>

class IrcConnection;

class ConnectionPlugin
{
public:
    virtual ~ConnectionPlugin() {}

    virtual void connectionAdded(IrcConnection*) {}
    virtual void connectionRemoved(IrcConnection*) {}

    QList<IrcConnection*> connections() const;
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);
};

Q_DECLARE_INTERFACE(ConnectionPlugin, "Communi.ConnectionPlugin")

#endif // CONNECTIONPLUGIN_H
