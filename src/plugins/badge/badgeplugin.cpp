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
#include <QHeaderView>
#include <IrcMessage>
#include <IrcBuffer>

BadgePlugin::BadgePlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
}

void BadgePlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
    d.tree->setColumnCount(2);
    d.tree->setItemDelegateForColumn(1, new BadgeDelegate(this));

    QHeaderView* header = tree->header();
    header->setStretchLastSection(false);
    header->setResizeMode(0, QHeaderView::Stretch);
    header->setResizeMode(1, QHeaderView::Fixed);
    header->resizeSection(1, tree->fontMetrics().width("999"));

    connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    connect(tree, SIGNAL(bufferRemoved(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));
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
            item->setBadge(item->badge() + 1);
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(BadgePlugin)
#endif
