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
#include "treerole.h"
#include "treewidget.h"
#include <IrcBuffer>

TreeItem::TreeItem(IrcBuffer* buffer, TreeItem* parent) : QObject(buffer), QTreeWidgetItem(parent)
{
    init(buffer);
}

TreeItem::TreeItem(IrcBuffer* buffer, TreeWidget* parent) : QObject(buffer), QTreeWidgetItem(parent)
{
    init(buffer);
}

void TreeItem::init(IrcBuffer* buffer)
{
    d.buffer = buffer;
    setObjectName(buffer->title());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(refresh()));
    connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
    connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(onBufferDestroyed()));
}

TreeItem::~TreeItem()
{
    d.buffer = 0;
    emit destroyed(this);
}

IrcBuffer* TreeItem::buffer() const
{
    return d.buffer;
}

IrcConnection* TreeItem::connection() const
{
    if (d.buffer)
        return d.buffer->connection();
    return 0;
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
    if (role == TreeRole::Active)
        return d.buffer->isActive();
    if (column == 0 && role == Qt::DisplayRole && d.buffer)
        return d.buffer->data(role);
    return QTreeWidgetItem::data(column, role);
}

bool TreeItem::operator<(const QTreeWidgetItem& other) const
{
    return treeWidget()->lessThan(this, static_cast<const TreeItem*>(&other));
}

void TreeItem::refresh()
{
    emitDataChanged();
}

void TreeItem::onBufferDestroyed()
{
    d.buffer = 0;
}
