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

#include "dock.h"
#include "alert.h"
#include "mainwindow.h"
#include "sharedtimer.h"
#include "qtdocktile.h"
#include <QDesktopServices>
#include <IrcTextFormat>
#include <IrcConnection>
#include <QApplication>
#include <IrcMessage>
#include <QFile>
#include <QDir>

Dock::Dock(MainWindow* window) : QObject(window)
{
    d.tray = 0;
    d.alert = 0;
    d.blink = false;
    d.blinking = false;
    d.window = window;

    connect(window, SIGNAL(activated()), this, SLOT(onWindowActivated()));
    connect(window, SIGNAL(connectionAdded(IrcConnection*)), this, SLOT(onConnectionAdded(IrcConnection*)));
    connect(window, SIGNAL(connectionRemoved(IrcConnection*)), this, SLOT(onConnectionRemoved(IrcConnection*)));

    if (QtDockTile::isAvailable())
        d.dock = new QtDockTile(window);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        d.tray = new QSystemTrayIcon(this);

#ifdef Q_OS_MAC
        d.alertIcon.addFile(":/images/tray/osx-alert.png");
        d.onlineIcon.addFile(":/images/tray/osx-online.png");
        d.offlineIcon.addFile(":/images/tray/osx-offline.png");
#else
        d.alertIcon.addFile(":/images/tray/alert-16.png");
        d.alertIcon.addFile(":/images/tray/alert-32.png");

        d.onlineIcon.addFile(":/images/tray/online-16.png");
        d.onlineIcon.addFile(":/images/tray/online-32.png");

        d.offlineIcon.addFile(":/images/tray/offline-16.png");
        d.offlineIcon.addFile(":/images/tray/offline-32.png");
#endif

        d.tray->setIcon(d.offlineIcon);
        d.tray->setVisible(true);

        connect(d.tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

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

void Dock::alert(IrcMessage* message)
{
    if (!d.window->isActiveWindow()) {
        QApplication::alert(d.window);
        if (d.alert)
            d.alert->play();
        if (d.tray && !d.blinking) {
            QString content = message->property("content").toString();
            if (!content.isEmpty())
                d.tray->showMessage(tr("Communi"), message->nick() + ": " + IrcTextFormat().toPlainText(content));
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
