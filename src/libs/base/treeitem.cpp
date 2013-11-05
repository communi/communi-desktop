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

#include "treeitem.h"
#include "treewidget.h"
#include <IrcBufferModel>
#include <IrcBuffer>

TreeItem::TreeItem(IrcBuffer* buffer, TreeItem* parent) : QTreeWidgetItem(parent)
{
    init(buffer);
}

TreeItem::TreeItem(IrcBuffer* buffer, TreeWidget* parent) : QTreeWidgetItem(parent)
{
    init(buffer);
}

void TreeItem::init(IrcBuffer* buffer)
{
    d.buffer = buffer;
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    connect(buffer, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(refresh()));
    connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
}

TreeItem::~TreeItem()
{
    emit destroyed(this);
}

IrcBuffer* TreeItem::buffer() const
{
    return d.buffer;
}

IrcConnection* TreeItem::connection() const
{
    return d.buffer->connection();
}

TreeItem* TreeItem::parentItem() const
{
    return static_cast<TreeItem*>(QTreeWidgetItem::parent());
}

TreeWidget* TreeItem::treeWidget() const
{
    return static_cast<TreeWidget*>(QTreeWidgetItem::treeWidget());
}

QVariant TreeItem::data(int column, int role) const
{
    if (column == 0 && role == Qt::DisplayRole)
        return d.buffer->data(role);
    return QTreeWidgetItem::data(column, role);
}

// TODO
class FriendlyModel : public IrcBufferModel
{
    friend class TreeItem;
};

bool TreeItem::operator<(const QTreeWidgetItem& other) const
{
    const TreeItem* otherItem = static_cast<const TreeItem*>(&other);
    if (!parentItem()) {
        QList<IrcConnection*> connections = treeWidget()->connections();
        return connections.indexOf(connection()) < connections.indexOf(otherItem->connection());
    }
    const FriendlyModel* model = static_cast<FriendlyModel*>(d.buffer->model());
    return model->lessThan(d.buffer, otherItem->buffer(), model->sortMethod());
}

void TreeItem::refresh()
{
    emitDataChanged();
}
