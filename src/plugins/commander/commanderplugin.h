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

#ifndef COMMANDERPLUGIN_H
#define COMMANDERPLUGIN_H

#include <QtPlugin>
#include <QStringList>
#include <IrcCommandFilter>
#include "connectionplugin.h"
#include "bufferplugin.h"
#include "viewplugin.h"

class IrcBuffer;
class IrcConnection;

class CommanderPlugin : public QObject, public IrcCommandFilter, public BufferPlugin, public ConnectionPlugin, public ViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(IrcCommandFilter BufferPlugin ConnectionPlugin ViewPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.BufferPlugin")
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.ViewPlugin")
#endif

public:
    CommanderPlugin(QObject* parent = 0);

    void initView(BufferView* view);
    void initBuffer(IrcBuffer* buffer);
    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);

    bool commandFilter(IrcCommand* command);

private:
    struct Private {
        QStringList chans;
    } d;
};

#endif // COMMANDERPLUGIN_H
