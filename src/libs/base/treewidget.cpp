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

#include "treewidget.h"
#include "treedelegate.h"
#include "sharedtimer.h"
#include "treeitem.h"
#include "treerole.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <QHeaderView>
#include <IrcMessage>
#include <IrcBuffer>
#include <QShortcut>
#include <QTimer>

TreeWidget::TreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    d.block = false;

    setAnimated(true);
    setColumnCount(2);
    setIndentation(0);
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setUniformRowHeights(true);
    setFocusPolicy(Qt::NoFocus);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setItemDelegate(new TreeDelegate(this));

    setSortingEnabled(true);
    setSortFunc(standardTreeSortFunc);
    sortByColumn(0, Qt::AscendingOrder);

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->resizeSection(1, fontMetrics().width("999"));

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    QShortcut* shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(resetItems()));
}

IrcBuffer* TreeWidget::currentBuffer() const
{
    TreeItem* item = static_cast<TreeItem*>(currentItem());
    if (item)
        return item->buffer();
    return 0;
}

TreeItem* TreeWidget::bufferItem(IrcBuffer* buffer) const
{
    return d.bufferItems.value(buffer);
}

TreeItem* TreeWidget::connectionItem(IrcConnection* connection) const
{
    return d.connectionItems.value(connection);
}

TreeSortFunc TreeWidget::sortFunc() const
{
    return d.sortFunc;
}

void TreeWidget::setSortFunc(TreeSortFunc func)
{
    d.sortFunc = func;
}

bool TreeWidget::blockItemReset(bool block)
{
    bool wasBlocked = d.block;
    if (d.block != block) {
        d.block = block;
        QTreeWidgetItem* current = currentItem();
        if (!block && current) {
            delayedResetBadge(current);
            unhighlightItem(current);
        }
    }
    return wasBlocked;
}

void TreeWidget::addBuffer(IrcBuffer* buffer)
{
    TreeItem* item = 0;
    if (buffer->isSticky()) {
        item = new TreeItem(buffer, this);
        item->setFirstColumnSpanned(true);
        item->setExpanded(true);
        IrcConnection* connection = buffer->connection();
        d.connectionItems.insert(connection, item);
        d.connections.append(connection);
    } else {
        TreeItem* parent = d.connectionItems.value(buffer->connection());
        item = new TreeItem(buffer, parent);
    }
    connect(item, SIGNAL(destroyed(TreeItem*)), this, SLOT(onItemDestroyed(TreeItem*)));
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
    d.bufferItems.insert(buffer, item);
    emit bufferAdded(buffer);
}

void TreeWidget::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky()) {
        IrcConnection* connection = buffer->connection();
        d.connectionItems.remove(connection);
        d.connections.removeOne(connection);
    }
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
    emit bufferRemoved(buffer);
    delete d.bufferItems.take(buffer);
}

void TreeWidget::setCurrentBuffer(IrcBuffer* buffer)
{
    TreeItem* item = d.bufferItems.value(buffer);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::closeBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = currentBuffer();
    if (buffer)
        emit bufferClosed(buffer);
}

QSize TreeWidget::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeWidget::sizeHint().height());
}

void TreeWidget::resetBadge(QTreeWidgetItem* item)
{
    if (!item && !d.resetBadges.isEmpty())
        item = d.resetBadges.dequeue();
    if (item)
        item->setData(1, TreeRole::Badge, 0);
}

void TreeWidget::delayedResetBadge(QTreeWidgetItem* item)
{
    d.resetBadges.enqueue(static_cast<TreeItem*>(item));
    QTimer::singleShot(500, this, SLOT(resetBadge()));
}

void TreeWidget::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        TreeItem* item = bufferItem(qobject_cast<IrcBuffer*>(sender()));
        if (item) {
            if (item != currentItem()) {
                if (message->nick() != QLatin1String("***") || message->ident() != QLatin1String("znc"))
                    item->setData(1, TreeRole::Badge, item->data(1, TreeRole::Badge).toInt() + 1);
                if (message->property("private").toBool() ||
                        message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive)) {
                    highlightItem(item);
                }
            }
        }
    }
}

void TreeWidget::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeWidget::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeWidget::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.block) {
        if (previous) {
            resetBadge(previous);
            unhighlightItem(previous);
        }
        if (current) {
            delayedResetBadge(current);
            unhighlightItem(current);
        }
    }

    TreeItem* item = static_cast<TreeItem*>(current);
    emit currentItemChanged(item);
    emit currentBufferChanged(item ? item->buffer() : 0);
}

void TreeWidget::onItemDestroyed(TreeItem* item)
{
    d.resetBadges.removeOne(item);
    d.highlightedItems.remove(item);
}

void TreeWidget::blinkItems()
{
    foreach (QTreeWidgetItem* item, d.highlightedItems)
        updateHighlight(item);
    d.blink = !d.blink;
}

void TreeWidget::resetItems()
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        resetBadge(*it);
        unhighlightItem(*it);
        ++it;
    }
}

void TreeWidget::highlightItem(QTreeWidgetItem* item)
{
    if (item && !d.highlightedItems.contains(item)) {
        if (d.highlightedItems.isEmpty())
            SharedTimer::instance()->registerReceiver(this, "blinkItems");
        d.highlightedItems.insert(item);
        updateHighlight(item);
    }
}

void TreeWidget::unhighlightItem(QTreeWidgetItem* item)
{
    if (item && d.highlightedItems.contains(item)) {
        d.highlightedItems.remove(item);
        if (d.highlightedItems.isEmpty())
            SharedTimer::instance()->unregisterReceiver(this, "blinkItems");
        updateHighlight(item);
    }
}

void TreeWidget::updateHighlight(QTreeWidgetItem* item)
{
    TreeItem* ti = static_cast<TreeItem*>(item);
    if (ti) {
        const bool hilite = d.blink && d.highlightedItems.contains(item);
        item->setData(0, TreeRole::Highlight, hilite);
        item->setData(1, TreeRole::Highlight, hilite);
        TreeItem* pi = ti->parentItem();
        if (pi)
            pi->setData(0, TreeRole::Highlight, hilite && !pi->isExpanded());
    }
}

// TODO
class FriendlyModel : public IrcBufferModel
{
    friend bool standardTreeSortFunc(const TreeItem* one, const TreeItem* another);
};

bool standardTreeSortFunc(const TreeItem* one, const TreeItem* another)
{
    if (!one->parentItem()) {
        QList<IrcConnection*> connections = one->treeWidget()->d.connections;
        return connections.indexOf(one->connection()) < connections.indexOf(another->connection());
    }
    if (one->buffer()) {
        const FriendlyModel* model = static_cast<FriendlyModel*>(one->buffer()->model());
        return model->lessThan(one->buffer(), another->buffer(), model->sortMethod());
    }
    return one->QTreeWidgetItem::operator<(*another);
}
