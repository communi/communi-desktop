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
#include "documentplugin.h"
#include "windowplugin.h"

class IrcBuffer;
class IrcMessage;

class TrayPlugin : public QObject, public ConnectionPlugin, public DocumentPlugin, public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin DocumentPlugin WindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")
#endif

public:
    TrayPlugin(QObject* parent = 0);

    void documentAdded(TextDocument* document);
    void documentRemoved(TextDocument* document);

    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

    void windowActivated();

private slots:
    void updateIcon();
    void onMessageHighlighted(IrcMessage* message);
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    struct Private {
        bool alert;
        bool blink;
        QIcon alertIcon;
        QIcon onlineIcon;
        QIcon offlineIcon;
        QSystemTrayIcon* tray;
        QSet<IrcConnection*> connections;
    } d;
};

#endif // TRAYPLUGIN_H
