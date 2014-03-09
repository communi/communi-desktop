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
#include "win/networkmonitor.h"

#include <qt_windows.h>
#include <qabstracteventdispatcher.h>

#if QT_VERSION >= 0x050000
#include <qabstractnativeeventfilter.h>
#endif // QT_VERSION

class SystemMonitorPrivate
#if QT_VERSION >= 0x050000
                            : public QAbstractNativeEventFilter
#endif
{
public:
#if QT_VERSION >= 0x050000
    bool nativeEventFilter(const QByteArray&, void* message, long*)
#else
    static bool nativeEventFilter(void* message)
#endif
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg && msg->message == WM_POWERBROADCAST) {
            switch (msg->wParam) {
            case PBT_APMSUSPEND:
                QMetaObject::invokeMethod(SystemMonitor::instance(), "sleep");
                break;
            case PBT_APMRESUMESUSPEND:
                QMetaObject::invokeMethod(SystemMonitor::instance(), "wake");
                break;
            default:
                break;
            }
        }
        return false;
    }
#if QT_VERSION < 0x050000
    QAbstractEventDispatcher::EventFilter prev;
#endif
    NetworkMonitor network;
};

void SystemMonitor::initialize()
{
    d = new SystemMonitorPrivate;
#if QT_VERSION >= 0x050000
    QAbstractEventDispatcher::instance()->installNativeEventFilter(d);
#else
    d->prev = QAbstractEventDispatcher::instance()->setEventFilter(SystemMonitorPrivate::nativeEventFilter);
#endif // QT_VERSION

    connect(&d->network, SIGNAL(online()), this, SIGNAL(online()));
    connect(&d->network, SIGNAL(offline()), this, SIGNAL(offline()));
}

void SystemMonitor::uninitialize()
{
    if (QAbstractEventDispatcher::instance())
#if QT_VERSION >= 0x050000
        QAbstractEventDispatcher::instance()->removeNativeEventFilter(d);
#else
        QAbstractEventDispatcher::instance()->setEventFilter(d->prev);
#endif // QT_VERSION
    delete d;
}
