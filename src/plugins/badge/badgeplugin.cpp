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

#include "badgeplugin.h"
#include "badgedelegate.h"
#include "treewidget.h"
#include "treeitem.h"
#include "treerole.h"
#include <QHeaderView>
#include <IrcMessage>
#include <IrcBuffer>
#include <QTimer>
#include <QEvent>

BadgePlugin::BadgePlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.block = false;
}

void BadgePlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
    d.tree->setColumnCount(2);
    d.tree->setItemDelegateForColumn(1, new BadgeDelegate(this));
    d.tree->installEventFilter(this);

    QHeaderView* header = tree->header();
    header->setStretchLastSection(false);
    header->setResizeMode(0, QHeaderView::Stretch);
    header->setResizeMode(1, QHeaderView::Fixed);
    header->resizeSection(1, tree->fontMetrics().width("999"));

    connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    connect(tree, SIGNAL(bufferRemoved(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));
    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

bool BadgePlugin::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::DynamicPropertyChange) {
        bool block = d.tree->property("blockItemReset").toBool();
        if (d.block != block) {
            d.block = block;
            QTreeWidgetItem* current = d.tree->currentItem();
            if (!block && current)
                delayedResetItem(current);
        }
    }
    return false;
}

void BadgePlugin::onBufferAdded(IrcBuffer* buffer)
{
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void BadgePlugin::onBufferRemoved(IrcBuffer* buffer)
{
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void BadgePlugin::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());

        TreeItem* item = d.tree->bufferItem(buffer);
        if (item && item != d.tree->currentItem())
            item->setData(1, TreeRole::Badge, item->data(1, TreeRole::Badge).toInt() + 1);
    }
}

void BadgePlugin::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.block) {
        if (previous)
            resetItem(previous);
        if (current)
            delayedResetItem(current);
    }
}

void BadgePlugin::resetItem(QTreeWidgetItem* item)
{
    if (!item && !d.items.isEmpty())
        item = d.items.dequeue();
    if (item)
        item->setData(1, TreeRole::Badge, 0);
}

void BadgePlugin::delayedResetItem(QTreeWidgetItem* item)
{
    d.items.enqueue(static_cast<TreeItem*>(item));
    QTimer::singleShot(500, this, SLOT(resetItem()));
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(BadgePlugin)
#endif
