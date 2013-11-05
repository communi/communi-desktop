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

TrayPlugin::TrayPlugin(QObject* parent) : QObject(parent)
{
    d.tray = 0;
    d.window = 0;
}

void TrayPlugin::initialize(QWidget* window)
{
    d.window = window;

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        d.tray = new QSystemTrayIcon(window->windowIcon(), window);
        d.tray->setVisible(true);
        connect(d.tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    }
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
