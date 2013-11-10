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

#include "monitorplugin.h"
#include "systemmonitor.h"
#include <IrcConnection>

MonitorPlugin::MonitorPlugin(QObject* parent) : QObject(parent)
{
}

void MonitorPlugin::initialize(IrcConnection* connection)
{
    connect(SystemMonitor::instance(), SIGNAL(wake()), connection, SLOT(open()));
    connect(SystemMonitor::instance(), SIGNAL(online()), connection, SLOT(open()));

    connect(SystemMonitor::instance(), SIGNAL(sleep()), connection, SLOT(quit()));
    connect(SystemMonitor::instance(), SIGNAL(sleep()), connection, SLOT(close()));

    connect(SystemMonitor::instance(), SIGNAL(offline()), connection, SLOT(quit()));
    connect(SystemMonitor::instance(), SIGNAL(offline()), connection, SLOT(close()));
}

void MonitorPlugin::uninitialize(IrcConnection* connection)
{
    disconnect(SystemMonitor::instance(), SIGNAL(wake()), connection, SLOT(open()));
    disconnect(SystemMonitor::instance(), SIGNAL(online()), connection, SLOT(open()));

    disconnect(SystemMonitor::instance(), SIGNAL(sleep()), connection, SLOT(quit()));
    disconnect(SystemMonitor::instance(), SIGNAL(sleep()), connection, SLOT(close()));

    disconnect(SystemMonitor::instance(), SIGNAL(offline()), connection, SLOT(quit()));
    disconnect(SystemMonitor::instance(), SIGNAL(offline()), connection, SLOT(close()));
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(MonitorPlugin)
#endif
