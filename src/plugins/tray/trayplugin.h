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

#ifndef TRAYPLUGIN_H
#define TRAYPLUGIN_H

#include <QSet>
#include <QObject>
#include <QPointer>
#include <QtPlugin>
#include <QSystemTrayIcon>
#include "connectionplugin.h"

class IrcMessage;

class TrayPlugin : public QObject, public ConnectionPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
#endif

public:
    TrayPlugin(QObject* parent = 0);

    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);

    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void updateIcon();
    void onMessageReceived(IrcMessage* message);
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    struct Private {
        bool alert;
        bool blink;
        QWidget* window;
        QIcon alertIcon;
        QIcon onlineIcon;
        QIcon offlineIcon;
        QSystemTrayIcon* tray;
        QSet<IrcConnection*> connections;
    } d;
};

#endif // TRAYPLUGIN_H
