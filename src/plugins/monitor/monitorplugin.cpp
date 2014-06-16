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

#include "monitorplugin.h"
#include "systemmonitor.h"
#include "bufferview.h"
#include <IrcConnection>

MonitorPlugin::MonitorPlugin(QObject* parent) : QObject(parent)
{
}

void MonitorPlugin::viewAdded(BufferView* view)
{
    if (!window) {
        window = view->window();
        SystemMonitor* monitor = new SystemMonitor(window);
        connect(monitor, SIGNAL(screenLocked()), window, SIGNAL(screenLocked()));
        connect(monitor, SIGNAL(screenUnlocked()), window, SIGNAL(screenUnlocked()));
        connect(monitor, SIGNAL(screenSaverStarted()), window, SIGNAL(screenSaverStarted()));
        connect(monitor, SIGNAL(screenSaverStopped()), window, SIGNAL(screenSaverStopped()));
    }
}

void MonitorPlugin::connectionAdded(IrcConnection* connection)
{
    SystemMonitor* monitor = new SystemMonitor(connection);

    connect(monitor, SIGNAL(wake()), connection, SLOT(open()));
    connect(monitor, SIGNAL(online()), connection, SLOT(open()));

    connect(monitor, SIGNAL(sleep()), connection, SLOT(quit()));
    connect(monitor, SIGNAL(sleep()), connection, SLOT(close()));

    connect(monitor, SIGNAL(offline()), connection, SLOT(close()));
}
