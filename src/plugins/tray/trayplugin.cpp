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

#include "trayplugin.h"
#include "sharedtimer.h"
#include "textdocument.h"
#include <IrcConnection>
#include <IrcTextFormat>
#include <IrcMessage>
#include <IrcBuffer>
#include <QWidget>
#include <QEvent>

TrayPlugin::TrayPlugin(QObject* parent) : QObject(parent)
{
    d.tray = 0;
    d.alert = false;
    d.blink = false;

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        d.tray = new QSystemTrayIcon(this);

        d.alertIcon.addFile(":/images/tray/alert-16.png");
        d.alertIcon.addFile(":/images/tray/alert-32.png");

        d.onlineIcon.addFile(":/images/tray/online-16.png");
        d.onlineIcon.addFile(":/images/tray/online-32.png");

        d.offlineIcon.addFile(":/images/tray/offline-16.png");
        d.offlineIcon.addFile(":/images/tray/offline-32.png");

        d.tray->setIcon(d.offlineIcon);
        d.tray->setVisible(true);

        connect(d.tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    }
}

void TrayPlugin::documentAdded(TextDocument* document)
{
    if (!document->isClone()) {
        connect(document, SIGNAL(messageMissed(IrcMessage*)), this, SLOT(alert(IrcMessage*)));
        connect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(alert(IrcMessage*)));
    }
}

void TrayPlugin::documentRemoved(TextDocument* document)
{
    if (!document->isClone()) {
        disconnect(document, SIGNAL(messageMissed(IrcMessage*)), this, SLOT(alert(IrcMessage*)));
        disconnect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(alert(IrcMessage*)));
    }
}

void TrayPlugin::connectionAdded(IrcConnection* connection)
{
    connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateIcon()));
    d.connections.insert(connection);
    updateIcon();
}

void TrayPlugin::connectionRemoved(IrcConnection* connection)
{
    disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateIcon()));
    d.connections.remove(connection);
    updateIcon();
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

void TrayPlugin::alert(IrcMessage* message)
{
    if (d.tray && !d.alert && !isActiveWindow()) {
        QString content = message->property("content").toString();
        if (!content.isEmpty())
            d.tray->showMessage(tr("Communi"), message->nick() + ": " + IrcTextFormat().toPlainText(content));
        SharedTimer::instance()->registerReceiver(this, "updateIcon");
        d.alert = true;
        d.blink = true;
        updateIcon();
    }
}

void TrayPlugin::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    QWidget* wnd = window();
    switch (reason) {
        case QSystemTrayIcon::DoubleClick:
            wnd->setVisible(!wnd->isVisible());
            break;
        case QSystemTrayIcon::Trigger:
            wnd->raise();
            wnd->activateWindow();
            break;
        default:
            break;
    }
}

void TrayPlugin::windowActivated()
{
    if (d.tray && d.alert) {
        SharedTimer::instance()->unregisterReceiver(this, "updateIcon");
        d.alert = false;
        d.blink = false;
        updateIcon();
    }
}
