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
#include "treeitem.h"
#include "treerole.h"
#include <IrcBufferModel>
#include <QHeaderView>
#include <IrcMessage>
#include <IrcBuffer>
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

    setSortingEnabled(true);
    setSortFunc(standardTreeSortFunc);
    sortByColumn(0, Qt::AscendingOrder);

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->resizeSection(1, fontMetrics().width("999"));

    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
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
        if (!block && current)
            delayedResetItem(current);
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

void TreeWidget::resetItem(QTreeWidgetItem* item)
{
    if (!item && !d.resetItems.isEmpty())
        item = d.resetItems.dequeue();
    if (item)
        item->setData(1, TreeRole::Badge, 0);
}

void TreeWidget::delayedResetItem(QTreeWidgetItem* item)
{
    d.resetItems.enqueue(static_cast<TreeItem*>(item));
    QTimer::singleShot(500, this, SLOT(resetItem()));
}

void TreeWidget::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        if (message->nick() != QLatin1String("***") || message->ident() != QLatin1String("znc")) {
            IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());

            TreeItem* item = bufferItem(buffer);
            if (item && item != currentItem())
                item->setData(1, TreeRole::Badge, item->data(1, TreeRole::Badge).toInt() + 1);
        }
    }
}

void TreeWidget::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.block) {
        if (previous)
            resetItem(previous);
        if (current)
            delayedResetItem(current);
    }

    TreeItem* item = static_cast<TreeItem*>(current);
    emit currentItemChanged(item);
    emit currentBufferChanged(item ? item->buffer() : 0);
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
