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
#include <IrcBufferModel>
#include <QHeaderView>
#include <IrcBuffer>

TreeWidget::TreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    setAnimated(true);
    setColumnCount(1);
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

    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*)));
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

void TreeWidget::addBuffer(IrcBuffer* buffer)
{
    TreeItem* item = 0;
    if (buffer->isSticky()) {
        item = new TreeItem(buffer, this);
        item->setExpanded(true);
        IrcConnection* connection = buffer->connection();
        d.connectionItems.insert(connection, item);
        d.connections.append(connection);
    } else {
        TreeItem* parent = d.connectionItems.value(buffer->connection());
        item = new TreeItem(buffer, parent);
    }
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
    emit bufferRemoved(buffer);
    delete d.bufferItems.take(buffer);
}

void TreeWidget::setCurrentBuffer(IrcBuffer* buffer)
{
    TreeItem* item = d.bufferItems.value(buffer);
    if (item)
        setCurrentItem(item);
}

QSize TreeWidget::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeWidget::sizeHint().height());
}

void TreeWidget::onCurrentItemChanged(QTreeWidgetItem* item)
{
    TreeItem* ti = static_cast<TreeItem*>(item);
    emit currentItemChanged(ti);
    emit currentBufferChanged(ti ? ti->buffer() : 0);
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
