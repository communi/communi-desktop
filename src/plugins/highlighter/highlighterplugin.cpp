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

#include "highlighterplugin.h"
#include "treewidget.h"
#include "treeitem.h"
#include <IrcMessage>
#include <IrcBuffer>
#include <QTimer>
#include <QEvent>

HighlighterPlugin::HighlighterPlugin(QObject* parent) : QObject(parent)
{
    d.blocked = false;
    d.tree = 0;
    d.shortcut = 0;
}

void HighlighterPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;

    connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    connect(tree, SIGNAL(bufferRemoved(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));
    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    d.shortcut = new QShortcut(tree);
    d.shortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(d.shortcut, SIGNAL(activated()), this, SLOT(resetItems()));
}

bool HighlighterPlugin::eventFilter(QObject *object, QEvent* event)
{
    Q_UNUSED(object);
    switch (event->type()) {
    case QEvent::WindowActivate:
        delayedReset(d.tree->currentItem());
        break;
    case QEvent::WindowBlocked:
        d.blocked = true;
        break;
    case QEvent::WindowUnblocked:
        d.blocked = false;
        delayedReset(d.tree->currentItem());
        break;
    default:
        break;
    }
    return false;
}

void HighlighterPlugin::onBufferAdded(IrcBuffer* buffer)
{
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void HighlighterPlugin::onBufferRemoved(IrcBuffer* buffer)
{
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void HighlighterPlugin::onMessageReceived(IrcMessage* message)
{
    IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());
    TreeItem* item = d.tree->bufferItem(buffer);
    if (item && item != d.tree->currentItem())
        item->setBadge(item->badge() + 1);
}

void HighlighterPlugin::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.blocked) {
        if (previous)
            static_cast<TreeItem*>(previous)->reset();
    }
    if (current)
        delayedReset(current);
}

void HighlighterPlugin::delayedReset(QTreeWidgetItem* item)
{
    if (item)
        QTimer::singleShot(500, static_cast<TreeItem*>(item), SLOT(reset()));
}

void HighlighterPlugin::resetItems()
{
    QTreeWidgetItemIterator it(d.tree);
    while (*it) {
        static_cast<TreeItem*>(*it)->reset();
        ++it;
    }
}
