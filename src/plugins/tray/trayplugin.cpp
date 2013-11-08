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

#include "trayplugin.h"
#include "sharedtimer.h"
#include <IrcConnection>

inline void initResource() { Q_INIT_RESOURCE(tray); }

TrayPlugin::TrayPlugin(QObject* parent) : QObject(parent)
{
    d.tray = 0;
    d.window = 0;
}

void TrayPlugin::initialize(IrcConnection* connection)
{
    if (d.tray) {
        connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateIcon()));
        d.connections.insert(connection);
        updateIcon();
    }
}

void TrayPlugin::uninitialize(IrcConnection* connection)
{
    if (d.tray) {
        disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateIcon()));
        d.connections.remove(connection);
        updateIcon();
    }
}

void TrayPlugin::initialize(QWidget* window)
{
    d.window = window;

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        d.tray = new QSystemTrayIcon(window);

        initResource();

        d.alertIcon.addFile(":/icons/alert-16.png");
        d.alertIcon.addFile(":/icons/alert-32.png");

        d.onlineIcon.addFile(":/icons/online-16.png");
        d.onlineIcon.addFile(":/icons/online-32.png");

        d.offlineIcon.addFile(":/icons/offline-16.png");
        d.offlineIcon.addFile(":/icons/offline-32.png");

        d.tray->setIcon(d.offlineIcon);
        d.tray->setVisible(true);

        connect(d.tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    }
}

void TrayPlugin::updateIcon()
{
    bool online = false;
    foreach (IrcConnection* connection, d.connections) {
        if (connection->isConnected()) {
            online = true;
            break;
        }
    }
    d.tray->setIcon(online ? d.onlineIcon : d.offlineIcon);
}

void TrayPlugin::onActivated(QSystemTrayIcon::ActivationReason reason)
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

/* TODO:
void MainWindow::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange)
        unalert();
}

void MainWindow::unalert()
{
    if (isActiveWindow()) {
        if (d.dockTile)
            d.dockTile->setBadge(0);
        if (d.trayIcon) {
            d.trayIcon->setIcon(d.normalIcon);
            SharedTimer::instance()->unregisterReceiver(this, "doAlert");
        }
    }
}

void MainWindow::doAlert()
{
    QIcon current = d.trayIcon->icon();
    if (current.cacheKey() == d.normalIcon.cacheKey())
        d.trayIcon->setIcon(d.alertIcon);
    else
        d.trayIcon->setIcon(d.normalIcon);
}
*/

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(TrayPlugin)
#endif
