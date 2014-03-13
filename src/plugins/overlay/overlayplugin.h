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

#ifndef OVERLAYPLUGIN_H
#define OVERLAYPLUGIN_H

#include <QtPlugin>
#include "connectionplugin.h"
#include "bufferviewplugin.h"

class Overlay;

class OverlayPlugin : public QObject, public ConnectionPlugin, public BufferViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin BufferViewPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.BufferViewPlugin")
#endif

public:
    OverlayPlugin(QObject* parent = 0);

    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);

    void initView(BufferView* view);
    void cleanupView(BufferView* view);

private slots:
    void reconnect();
    void updateOverlays();

private:
    struct Private {
        QList<Overlay*> overlays;
    } d;
};

#endif // OVERLAYPLUGIN_H
