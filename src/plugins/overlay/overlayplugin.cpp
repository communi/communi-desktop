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

#include "overlayplugin.h"
#include "mainwindow.h"
#include "bufferview.h"
#include "overlay.h"
#include <IrcConnection>
#include <IrcBuffer>

inline void initResource() { Q_INIT_RESOURCE(overlay); }

OverlayPlugin::OverlayPlugin(QObject* parent) : QObject(parent)
{
    d.window = 0;
    initResource();
}

void OverlayPlugin::initConnection(IrcConnection* connection)
{
    connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateOverlays()));
}

void OverlayPlugin::cleanupConnection(IrcConnection* connection)
{
    disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateOverlays()));
}

void OverlayPlugin::initView(BufferView* view)
{
    Overlay* overlay = new Overlay(view);
    connect(overlay, SIGNAL(refresh()), this, SLOT(reconnect()));
    d.overlays += overlay;

    connect(view, SIGNAL(bufferChanged(IrcBuffer*)), this, SLOT(updateOverlays()));
}

void OverlayPlugin::cleanupView(BufferView* view)
{
    d.overlays.removeOne(view->findChild<Overlay*>());
    disconnect(view, SIGNAL(bufferChanged(IrcBuffer*)), this, SLOT(updateOverlays()));
}

void OverlayPlugin::initWindow(MainWindow* window)
{
    d.window = window;
}

void OverlayPlugin::reconnect()
{
    Overlay* overlay = qobject_cast<Overlay*>(sender());
    if (overlay) {
        BufferView* view = qobject_cast<BufferView*>(overlay->parentWidget());
        if (view) { // TODO
            IrcBuffer* buffer = view->buffer();
            if (buffer) { // TODO
                IrcConnection* connection = buffer->connection();
                if (connection) {
                    connection->setEnabled(true);
                    connection->open();
                }
            }
        }
    }
}

void OverlayPlugin::updateOverlays()
{
    foreach (Overlay* overlay, d.overlays) {
        BufferView* view = qobject_cast<BufferView*>(overlay->parentWidget());
        if (view) {
            IrcBuffer* buffer = view->buffer();
            if (buffer) { // TODO
                IrcConnection* connection = buffer->connection();
                if (connection) { // TODO
                    overlay->setBusy(connection->isActive() && !connection->isConnected());
                    overlay->setRefresh(!connection->isActive());
                    overlay->setVisible(!connection->isConnected());
                    // TODO: overlay->setDark(Application::settings()->value("ui.dark").toBool());
                    if (!connection->isConnected() && d.window->currentView())
                        overlay->setFocus();
                }
            }
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(OverlayPlugin)
#endif
