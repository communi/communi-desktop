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

#ifndef COMMANDVERIFIER_H
#define COMMANDVERIFIER_H

#include <QMap>
#include <IrcMessageFilter>
#include <IrcCommandFilter>

class IrcConnection;

class CommandVerifier : public QObject, public IrcMessageFilter, public IrcCommandFilter
{
    Q_OBJECT
    Q_INTERFACES(IrcCommandFilter IrcMessageFilter)

public:
    CommandVerifier(IrcConnection* connection);

    int identify(IrcMessage* message) const;

    bool messageFilter(IrcMessage* message);
    bool commandFilter(IrcCommand* command);

signals:
    void verified(int id);

private:
    struct Private {
        static int id;
        IrcConnection* connection;
        QMap<int, IrcCommand*> commands;
    } d;
};

#endif // COMMANDVERIFIER_H
