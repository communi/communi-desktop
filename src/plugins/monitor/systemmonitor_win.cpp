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
#include "win/screenmonitor.h"
#include "win/networkmonitor.h"

#include <qt_windows.h>
#include <qabstracteventdispatcher.h>
#include <qabstractnativeeventfilter.h>

class SystemMonitorPrivate : public QAbstractNativeEventFilter
{
public:
    SystemMonitorPrivate(SystemMonitor* monitor) : monitor(monitor)
    {
    }

    bool nativeEventFilter(const QByteArray&, void* message, long*)
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg && msg->message == WM_POWERBROADCAST) {
            switch (msg->wParam) {
            case PBT_APMSUSPEND:
                QMetaObject::invokeMethod(monitor, "sleep");
                break;
            case PBT_APMRESUMESUSPEND:
                QMetaObject::invokeMethod(monitor, "wake");
                break;
            default:
                break;
            }
        }
        return false;
    }

    SystemMonitor* monitor;
    NetworkMonitor network;
    ScreenMonitor screen;
};

void SystemMonitor::initialize()
{
    d = new SystemMonitorPrivate(this);
    QAbstractEventDispatcher::instance()->installNativeEventFilter(d);

    connect(&d->network, SIGNAL(online()), this, SIGNAL(online()));
    connect(&d->network, SIGNAL(offline()), this, SIGNAL(offline()));

    connect(&d->screen, SIGNAL(screenLocked()), this, SIGNAL(screenLocked()));
    connect(&d->screen, SIGNAL(screenUnlocked()), this, SIGNAL(screenUnlocked()));
    connect(&d->screen, SIGNAL(screenSaverStarted()), this, SIGNAL(screenSaverStarted()));
    connect(&d->screen, SIGNAL(screenSaverStopped()), this, SIGNAL(screenSaverStopped()));
}

void SystemMonitor::uninitialize()
{
    if (QAbstractEventDispatcher::instance())
        QAbstractEventDispatcher::instance()->removeNativeEventFilter(d);
    delete d;
}
