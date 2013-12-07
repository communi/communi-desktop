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

#include "triconplugin.h"
#include "treewidget.h"
#include "treeitem.h"
#include <IrcConnection>
#include <IrcLagTimer>
#include <QPainter>
#include <QPixmap>
#include <qmath.h>

inline void initResource() { Q_INIT_RESOURCE(tricon); }

TriconPlugin::TriconPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;

    initResource();
    d.movie.setFileName(":/ajax-loader.gif");
    connect(&d.movie, SIGNAL(frameChanged(int)), this, SLOT(updateLoader()));
}

void TriconPlugin::initTree(TreeWidget* tree)
{
    d.tree = tree;
    foreach (IrcConnection* connection, d.connections)
        updateConnection(connection);
    d.connections.clear();
}

void TriconPlugin::initConnection(IrcConnection* connection)
{
    connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateConnection()));
    if (d.tree)
        updateConnection(connection);
    else
        d.connections += connection;

    IrcLagTimer* timer = new IrcLagTimer(connection);
    connect(timer, SIGNAL(lagChanged(qint64)), this, SLOT(updateLag(qint64)));
    // TODO:
    connect(connection, SIGNAL(connected()), timer, SLOT(_irc_pingServer()));
}

void TriconPlugin::cleanupConnection(IrcConnection* connection)
{
    disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateConnection()));
    foreach (TreeItem* item, d.items) {
        if (!item || item->connection() == connection)
            d.items.removeOne(item);
    }
}

void TriconPlugin::updateLoader()
{
    QPixmap pixmap = d.movie.currentPixmap();
    foreach (TreeItem* item, d.items) {
        if (item)
            item->setIcon(0, pixmap);
    }
}

void TriconPlugin::updateLag(qint64 lag)
{
    IrcLagTimer* timer = qobject_cast<IrcLagTimer*>(sender());
    if (timer)
        updateConnection(timer->connection(), lag);
}

void TriconPlugin::updateConnection(IrcConnection* connection, qint64 lag)
{
    if (!connection)
        connection = qobject_cast<IrcConnection*>(sender());
    TreeItem* item = d.tree->connectionItem(connection);
    if (item) {
        item->setToolTip(0, lag > 0 ? tr("%1ms").arg(lag) : QString());
        if (connection->isActive() && !connection->isConnected()) {
            item->setIcon(0, d.movie.currentPixmap());
            if (!d.items.contains(item))
                d.items.append(item);
        } else {
            QPixmap pixmap(16, 16);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setPen(QPalette().color(QPalette::Mid));
            QColor color(Qt::transparent);
            if (lag > 0) {
                qreal f = qMin(100.0, qSqrt(lag)) / 100;
                color = QColor::fromHsl(120 - f * 120, 96, 152); // TODO
            }
            painter.setBrush(color);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawEllipse(4, 5, 8, 8);
            item->setIcon(0, pixmap);
            d.items.removeOne(item);
        }
    }

    if (d.items.isEmpty()) {
        if (d.movie.state() == QMovie::Running)
            d.movie.stop();
    } else {
        if (d.movie.state() == QMovie::NotRunning)
            d.movie.start();
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(TriconPlugin)
#endif
