/*
  Copyright (C) 2008-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
