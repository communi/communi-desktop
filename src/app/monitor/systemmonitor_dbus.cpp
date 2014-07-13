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

#include "systemmonitor.h"
#include <QtDBus>

class SystemMonitorPrivate : public QObject
{
    Q_OBJECT

public:
    SystemMonitorPrivate(SystemMonitor* monitor) : monitor(monitor)
    {
    }

private slots:
    void sleeping()
    {
        QMetaObject::invokeMethod(monitor, "sleep");
    }

    void resuming()
    {
        QMetaObject::invokeMethod(monitor, "wake");
    }

    void networkStateChanged(uint state)
    {
        static const uint NM_STATE_DISCONNECTED = 20;
        static const uint NM_STATE_CONNECTED_GLOBAL = 70;

        if (state == NM_STATE_DISCONNECTED)
            QMetaObject::invokeMethod(monitor, "offline");
        else if (state == NM_STATE_CONNECTED_GLOBAL)
            QMetaObject::invokeMethod(monitor, "online");
    }

    void screenSaverActiveChanged(bool active)
    {
        if (active)
            QMetaObject::invokeMethod(monitor, "screenSaverStarted");
        else
            QMetaObject::invokeMethod(monitor, "screenSaverStopped");
    }

private:
    SystemMonitor* monitor;
};

void SystemMonitor::initialize()
{
    d = new SystemMonitorPrivate(this);

    QDBusConnection system = QDBusConnection::systemBus();
    system.connect("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager",
                   "org.freedesktop.NetworkManager", "StateChanged", d, SLOT(networkStateChanged(uint)));

    system.connect("org.freedesktop.UPower", "/org/freedesktop/UPower",
                   "org.freedesktop.UPower", "Sleeping", d, SLOT(sleeping()));
    system.connect("org.freedesktop.UPower", "/org/freedesktop/UPower",
                   "org.freedesktop.UPower", "Resuming", d, SLOT(resuming()));

    QDBusConnection session = QDBusConnection::sessionBus();
    session.connect("org.freedesktop.ScreenSaver", "/ScreenSaver",
                    "org.freedesktop.ScreenSaver", "ActiveChanged", d, SLOT(screenSaverActiveChanged(bool)));
    session.connect("org.gnome.ScreenSaver", "/org/gnome/ScreenSaver",
                    "org.gnome.ScreenSaver", "ActiveChanged", d, SLOT(screenSaverActiveChanged(bool)));
}

void SystemMonitor::uninitialize()
{
    delete d;
}

#include "systemmonitor_dbus.moc"
