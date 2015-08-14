/*
  Copyright (C) 2008-2015 The Communi Project

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

#include "dock.h"
#include "alert.h"
#include "mainwindow.h"
#include "sharedtimer.h"
#include "qtdocktile.h"
#include "pluginloader.h"
#include <QDesktopServices>
#include <IrcTextFormat>
#include <IrcConnection>
#include <QApplication>
#include <IrcMessage>
#include <QSettings>
#include <QMenu>
#include <QFile>
#include <QDir>

Dock::Dock(MainWindow* window) : QObject(window)
{
    d.tray = 0;
    d.muteAction = 0;
    d.offlineAction = 0;
    d.alert = 0;
    d.blink = false;
    d.blinking = false;
    d.window = window;
    d.active = false;

    connect(window, SIGNAL(activated()), this, SLOT(onWindowActivated()));
    connect(window, SIGNAL(connectionAdded(IrcConnection*)), this, SLOT(onConnectionAdded(IrcConnection*)));
    connect(window, SIGNAL(connectionRemoved(IrcConnection*)), this, SLOT(onConnectionRemoved(IrcConnection*)));

    if (QtDockTile::isAvailable())
        d.dock = new QtDockTile(window);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        d.tray = new QSystemTrayIcon(this);

        init();

        QMenu* menu = new QMenu(window);
        d.tray->setContextMenu(menu);

        d.muteAction = menu->addAction(tr("Mute"));
        d.muteAction->setCheckable(true);

        d.offlineAction = menu->addAction(tr("Offline"));
        d.offlineAction->setCheckable(true);

        QSettings settings;
        d.muteAction->setChecked(settings.value("mute", false).toBool());
        connect(d.muteAction, SIGNAL(toggled(bool)), this, SLOT(onMuteToggled(bool)));
        d.offlineAction->setChecked(settings.value("offline", false).toBool());
        connect(d.offlineAction, SIGNAL(toggled(bool)), this, SLOT(onOfflineToggled(bool)));

        d.tray->setIcon(d.offlineIcon);
        d.tray->setVisible(true);

        connect(d.tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));
        connect(d.tray, SIGNAL(messageClicked()), this, SLOT(onTrayMessageClicked()));

        PluginLoader::instance()->setupTrayIcon(d.tray);
        PluginLoader::instance()->setupMuteAction(d.muteAction);

        updateTray();
    }

    if (Alert::isAvailable()) {
        d.alert = new Alert(this);

        QDir dataDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
        if (dataDir.exists() || dataDir.mkpath(".")) {
            QString filePath = dataDir.filePath("alert.mp3");
            if (!QFile::exists(filePath))
                QFile::copy(":/alert.mp3", filePath);
            d.alert->setFilePath(filePath);
        }
    }
}

Dock::~Dock()
{
    uninit();
}

void Dock::alert(IrcMessage* message)
{
    if (!d.window->isActiveWindow() || d.active) {
        QApplication::alert(d.window);
        if (d.alert && (!d.muteAction || !d.muteAction->isChecked()))
            d.alert->play();
        if (d.tray && !d.blinking) {
            PluginLoader::instance()->dockAlert(message);
            SharedTimer::instance()->registerReceiver(this, "updateTray");
            d.blinking = true;
            d.blink = true;
            updateTray();
        }
        updateBadge();
    }
}

void Dock::onConnectionAdded(IrcConnection* connection)
{
    connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateTray()));
    updateTray();
}

void Dock::onConnectionRemoved(IrcConnection* connection)
{
    disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateTray()));
    updateTray();
}

void Dock::updateBadge()
{
    if (d.dock)
        d.dock->setBadge(d.dock->badge() + 1);
}

void Dock::updateTray()
{
    if (d.tray) {
        bool online = false;
        foreach (IrcConnection* connection, d.window->connections()) {
            if (connection->isConnected()) {
                online = true;
                break;
            }
        }
        d.tray->setIcon(d.blinking && d.blink ? d.alertIcon : online ? d.onlineIcon : d.offlineIcon);
        d.blink = !d.blink;
    }
}

void Dock::activateAlert()
{
    d.active = true;
}

void Dock::deactivateAlert()
{
    d.active = false;
}

void Dock::onWindowActivated()
{
    if (d.tray && d.blinking) {
        SharedTimer::instance()->unregisterReceiver(this, "updateTray");
        d.blinking = false;
        d.blink = false;
        updateTray();
    }
    if (d.dock)
        d.dock->setBadge(0);
}

void Dock::onMuteToggled(bool mute)
{
    QSettings settings;
    settings.setValue("mute", mute);
}

void Dock::onOfflineToggled(bool offline)
{
    QSettings settings;
    settings.setValue("offline", offline);
    if (d.window) {
        foreach (IrcConnection* connection, d.window->connections()) {
            if (offline) {
                if (connection->isActive())
                    connection->quit(qApp->property("description").toString());
            } else {
                if (!connection->isActive())
                    connection->open();
            }
        }
    }
}

void Dock::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::DoubleClick:
            d.window->setVisible(!d.window->isVisible());
            break;
        case QSystemTrayIcon::Trigger:
            d.window->raise();
            d.window->activateWindow();
            break;
        default:
            break;
    }
}

void Dock::onTrayMessageClicked()
{
    d.window->show();
    d.window->raise();
    d.window->activateWindow();
}

#ifndef Q_OS_MAC
void Dock::init()
{
    d.alertIcon.addFile(":/images/tray/white/black.png");
    d.onlineIcon.addFile(":/images/tray/white/blue.png");
    d.offlineIcon.addFile(":/images/tray/white/gray.png");
}

void Dock::uninit()
{
}
#endif
