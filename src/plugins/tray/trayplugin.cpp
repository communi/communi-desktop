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
#include "mainwindow.h"
#include <IrcConnection>
#include <IrcMessage>
#include <IrcBuffer>
#include <QEvent>

inline void initResource() { Q_INIT_RESOURCE(tray); }

TrayPlugin::TrayPlugin(QObject* parent) : QObject(parent)
{
    d.tray = 0;
    d.window = 0;
    d.alert = false;
    d.blink = false;
}

void TrayPlugin::initialize(IrcConnection* connection)
{
    connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateIcon()));
    connect(connection, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
    d.connections.insert(connection);
    updateIcon();
}

void TrayPlugin::uninitialize(IrcConnection* connection)
{
    disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateIcon()));
    disconnect(connection, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
    d.connections.remove(connection);
    updateIcon();
}

void TrayPlugin::initialize(MainWindow* window)
{
    d.window = window;
    window->installEventFilter(this);

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
    if (d.tray) {
        bool online = false;
        foreach (IrcConnection* connection, d.connections) {
            if (connection->isConnected()) {
                online = true;
                break;
            }
        }
        d.tray->setIcon(d.alert && d.blink ? d.alertIcon : online ? d.onlineIcon : d.offlineIcon);
        d.blink = !d.blink;
    }
}

void TrayPlugin::onMessageReceived(IrcMessage* message)
{
    if (d.tray && !d.alert && !d.window->isActiveWindow()) {
        if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
            if (message->property("private").toBool() ||
                message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive)) {
                SharedTimer::instance()->registerReceiver(this, "updateIcon");
                d.alert = true;
                d.blink = true;
                updateIcon();
            }
        }
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

bool TrayPlugin::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ActivationChange) {
        if (d.tray && d.alert && d.window->isActiveWindow()) {
            SharedTimer::instance()->unregisterReceiver(this, "updateIcon");
            d.alert = false;
            d.blink = false;
            updateIcon();
        }
    }
    return false;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(TrayPlugin)
#endif
