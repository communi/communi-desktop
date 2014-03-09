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

#include "dockplugin.h"
#include "qtdocktile.h"
#include "mainwindow.h"
#include <IrcConnection>
#include <QEvent>

DockPlugin::DockPlugin(QObject* parent) : QObject(parent)
{
    d.dock = 0;
    d.window = 0;
}

void DockPlugin::initWindow(MainWindow* window)
{
    d.window = window;
    window->installEventFilter(this);

    if (QtDockTile::isAvailable())
        d.dock = new QtDockTile(window);
}

void DockPlugin::initConnection(IrcConnection* connection)
{
    if (d.dock)
        connect(connection, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void DockPlugin::cleanupConnection(IrcConnection* connection)
{
    if (d.dock)
        disconnect(connection, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void DockPlugin::onMessageReceived(IrcMessage* message)
{
    if (d.dock && !d.window->isActiveWindow()) {
        if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
            if (message->property("private").toBool() ||
                message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive)) {
                d.dock->setBadge(d.dock->badge() + 1);
            }
        }
    }
}

bool DockPlugin::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ActivationChange) {
        if (d.dock && d.window->isActiveWindow())
            d.dock->setBadge(0);
    }
    return false;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(DockPlugin)
#endif
