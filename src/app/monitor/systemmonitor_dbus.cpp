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

private:
    SystemMonitor* monitor;
};

void SystemMonitor::initialize()
{
    d = new SystemMonitorPrivate(this);

    QDBusConnection bus = QDBusConnection::systemBus();
    bus.connect("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager",
                "org.freedesktop.NetworkManager", "StateChanged", d, SLOT(networkStateChanged(uint)));

    bus.connect("org.freedesktop.UPower", "/org/freedesktop/UPower",
                "org.freedesktop.UPower", "Sleeping", d, SLOT(sleeping()));
    bus.connect("org.freedesktop.UPower", "/org/freedesktop/UPower",
                "org.freedesktop.UPower", "Resuming", d, SLOT(resuming()));
}

void SystemMonitor::uninitialize()
{
    delete d;
}

#include "systemmonitor_dbus.moc"
