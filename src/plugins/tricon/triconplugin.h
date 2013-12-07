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

#ifndef TRICONPLUGIN_H
#define TRICONPLUGIN_H

#include <QList>
#include <QMovie>
#include <QPointer>
#include <QtPlugin>
#include "connectionplugin.h"
#include "treewidgetplugin.h"

class TreeItem;
class IrcConnection;

class TriconPlugin : public QObject, public ConnectionPlugin, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    TriconPlugin(QObject* parent = 0);

    void initTree(TreeWidget* tree);

    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);

private slots:
    void updateLoader();
    void updateLag(qint64 lag);
    void updateConnection(IrcConnection* connection = 0, qint64 lag = -1);

private:
    struct Private {
        QMovie movie;
        TreeWidget* tree;
        QList<QPointer<TreeItem> > items;
        QList<QPointer<IrcConnection> > connections;
    } d;
};

#endif // TRICONPLUGIN_H
