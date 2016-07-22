/*
  Copyright (C) 2008-2016 The Communi Project

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
#include <QtDBus>

class SystemMonitorPrivate : public QObject
{
    Q_OBJECT

public:
    SystemMonitorPrivate(SystemMonitor* monitor) : monitor(monitor)
    {
    }

private slots:
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
                   "org.freedesktop.UPower", "Sleeping", this, SIGNAL(sleep()));
    system.connect("org.freedesktop.UPower", "/org/freedesktop/UPower",
                   "org.freedesktop.UPower", "Resuming", this, SIGNAL(wake()));

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
