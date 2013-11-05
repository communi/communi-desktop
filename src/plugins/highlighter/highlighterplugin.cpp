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
#include "textdocument.h"
#include "treewidget.h"
#include "treeitem.h"
#include "treerole.h"
#include <IrcConnection>
#include <IrcMessage>
#include <IrcBuffer>
#include <QTimer>
#include <QEvent>

HighlighterPlugin::HighlighterPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.shortcut = 0;
}

void HighlighterPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;

    connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    connect(tree, SIGNAL(bufferRemoved(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));
    connect(tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    d.shortcut = new QShortcut(tree);
    d.shortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(d.shortcut, SIGNAL(activated()), this, SLOT(resetItems()));
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
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());

        if (message->property("content").toString().contains(message->connection()->nickName())) {
            TreeItem* item = d.tree->bufferItem(buffer);
            if (item && item != d.tree->currentItem())
                highlightItem(item, true);

            TextDocument* document = buffer->property("document").value<TextDocument*>();
            document->addHighlight(document->totalCount() - 2); // TODO: -2??
        }
    }
}

void HighlighterPlugin::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void HighlighterPlugin::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void HighlighterPlugin::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    highlightItem(previous, false);
    highlightItem(current, false);
}

void HighlighterPlugin::highlightItem(QTreeWidgetItem* item, bool highlight)
{
    if (item)
        item->setData(0, Qt::ForegroundRole, highlight ? QColor("#ff4040") : QVariant());
}

void HighlighterPlugin::resetItems()
{
    QTreeWidgetItemIterator it(d.tree);
    while (*it) {
        highlightItem(*it, false);
        ++it;
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(HighlighterPlugin)
#endif
