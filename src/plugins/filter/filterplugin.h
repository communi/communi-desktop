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

#ifndef FILTERPLUGIN_H
#define FILTERPLUGIN_H

#include <QPair>
#include <QHash>
#include <QString>
#include <QtPlugin>
#include <QDateTime>
#include <IrcCommandFilter>
#include <IrcMessageFilter>
#include "connectionplugin.h"

class FilterPlugin : public QObject, public ConnectionPlugin, public IrcMessageFilter, public IrcCommandFilter
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin IrcCommandFilter IrcMessageFilter)
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")

public:
    FilterPlugin(QObject* parent = 0);

    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

    bool commandFilter(IrcCommand* command);
    bool messageFilter(IrcMessage* message);

private:
    struct Private {
        QHash<int, QPair<QDateTime, QString> > sentCommands;
        QHash<QString, QPair<QDateTime, QString> > awayReplies;
    } d;
};

#endif // FILTERPLUGIN_H
