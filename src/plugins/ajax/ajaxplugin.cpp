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

#include "ajaxplugin.h"
#include "treewidget.h"
#include "treeitem.h"
#include <IrcConnection>
#include <QPixmap>

inline void initResource() { Q_INIT_RESOURCE(ajax); }

AjaxPlugin::AjaxPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.movie = 0;
}

void AjaxPlugin::initTree(TreeWidget* tree)
{
    d.tree = tree;

    initResource();

    d.movie = new QMovie(tree);
    d.movie->setFileName(":/ajax-loader.gif");
    connect(d.movie, SIGNAL(frameChanged(int)), this, SLOT(updateLoader()));
}

void AjaxPlugin::updateLoader()
{
    QPixmap pixmap = d.movie->currentPixmap();
    foreach (TreeItem* item, d.items)
        item->setIcon(1, pixmap);
}

void AjaxPlugin::updateConnection()
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection) {
        TreeItem* item = d.tree->connectionItem(connection);
        if (item) {
            if (connection->isActive() && !connection->isConnected()) {
                item->setIcon(1, d.movie->currentPixmap());
                d.items.insert(item);
            } else {
                item->setIcon(1, QIcon());
                d.items.remove(item);
            }
        }
    }

    if (d.items.isEmpty()) {
        if (d.movie->state() == QMovie::Running)
            d.movie->stop();
    } else {
        if (d.movie->state() == QMovie::NotRunning)
            d.movie->start();
    }
}

void AjaxPlugin::initConnection(IrcConnection* connection)
{
    connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateConnection()));
}

void AjaxPlugin::cleanupConnection(IrcConnection* connection)
{
    disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateConnection()));
    foreach (TreeItem* item, d.items) {
        if (item->connection() == connection)
            d.items.remove(item);
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(AjaxPlugin)
#endif
