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

#ifndef COMMANDERPLUGIN_H
#define COMMANDERPLUGIN_H

#include <QtPlugin>
#include <QStringList>
#include <IrcCommandFilter>
#include "connectionplugin.h"
#include "bufferviewplugin.h"
#include "splitviewplugin.h"
#include "treewidgetplugin.h"

class IrcBuffer;
class IrcConnection;

class CommanderPlugin : public QObject, public IrcCommandFilter, public ConnectionPlugin,
                        public BufferViewPlugin, public SplitViewPlugin, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(IrcCommandFilter ConnectionPlugin BufferViewPlugin SplitViewPlugin TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.BufferViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.SplitViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    CommanderPlugin(QObject* parent = 0);

    void initView(BufferView* view);
    void initView(SplitView* view);
    void initTree(TreeWidget* tree);
    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);

    bool commandFilter(IrcCommand* command);

private slots:
    void onBufferAdded(IrcBuffer* buffer);

private:
    struct Private {
        SplitView* view;
        TreeWidget* tree;
        QStringList chans;
    } d;
};

#endif // COMMANDERPLUGIN_H
