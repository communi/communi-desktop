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

#include "lagplugin.h"
#include "treewidget.h"
#include "treeitem.h"
#include <IrcConnection>
#include <IrcLagTimer>
#include <QPainter>
#include <QPixmap>
#include <qmath.h>

LagPlugin::LagPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
}

void LagPlugin::initConnection(IrcConnection* connection)
{
    connect(connection, SIGNAL(connected()), this, SLOT(updateConnection()));

    IrcLagTimer* timer = new IrcLagTimer(connection);
    connect(timer, SIGNAL(lagChanged(qint64)), this, SLOT(updateLag(qint64)));
    // TODO:
    connect(connection, SIGNAL(connected()), timer, SLOT(_irc_pingServer()));
}

void LagPlugin::initTree(TreeWidget* tree)
{
    d.tree = tree;
}

void LagPlugin::updateLag(qint64 lag)
{
    IrcLagTimer* timer = qobject_cast<IrcLagTimer*>(sender());
    if (timer)
        updateConnection(timer->connection(), lag);
}

void LagPlugin::updateConnection(IrcConnection* connection, qint64 lag)
{
    if (!connection)
        connection = qobject_cast<IrcConnection*>(sender());
    TreeItem* item = d.tree->connectionItem(connection);
    if (item) {
        item->setToolTip(1, lag > 0 ? tr("%1ms").arg(lag) : QString());
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
        painter.drawEllipse(4, 4, 8, 8);
        item->setIcon(1, pixmap);
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(LagPlugin)
#endif
