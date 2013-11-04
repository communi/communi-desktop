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
#include "sharedtimer.h"
#include <IrcBufferModel>
#include <IrcBuffer>

TreeItem::TreeItem(IrcBuffer* buffer, TreeItem* parent) : QTreeWidgetItem(parent)
{
    init(buffer);
    connect(this, SIGNAL(destroyed(TreeItem*)), parent, SLOT(removeChild(TreeItem*)));
}

TreeItem::TreeItem(IrcBuffer* buffer, TreeWidget* parent) : QTreeWidgetItem(parent)
{
    init(buffer);
}

void TreeItem::init(IrcBuffer* buffer)
{
    d.badge = 0;
    d.blink = false;
    d.buffer = buffer;
    d.highlighted = false;
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

int TreeItem::badge() const
{
    return d.badge;
}

void TreeItem::setBadge(int badge)
{
    if (d.badge != badge) {
        d.badge = badge;
        emitDataChanged();
    }
}

bool TreeItem::isHighlighted() const
{
    return d.highlighted;
}

void TreeItem::setHighlighted(bool highlighted)
{
    if (d.highlighted != highlighted) {
        d.highlighted = highlighted;
        if (TreeItem* p = parentItem()) {
            if (highlighted)
                p->d.highlightedChildren.insert(this);
            else
                p->d.highlightedChildren.remove(this);
            if (!p->isExpanded())
                p->emitDataChanged();
        }
        if (highlighted)
            SharedTimer::instance()->registerReceiver(this, "blink");
        else
            SharedTimer::instance()->unregisterReceiver(this, "blink");
        emitDataChanged();
    }
}

QVariant TreeItem::data(int column, int role) const
{
    switch (role) {
    case TreeRole::Badge:
        return d.badge;
    case TreeRole::Highlight:
        return d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty());
    case Qt::BackgroundRole:
        if (column == 1 && d.blink && (d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty())))
            return QColor("#ff4040").lighter(125); // TODO
        return QTreeWidgetItem::data(column, role);
    case Qt::ForegroundRole:
        if (!d.buffer->isActive())
            return treeWidget()->palette().color(QPalette::Disabled, QPalette::Text);
        if (d.blink && (d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty())))
            return QColor("#ff4040"); // TODO
        return QTreeWidgetItem::data(column, role);
    default:
        if (column == 0 && d.buffer)
            return d.buffer->data(role);
        return QVariant();
    }
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

void TreeItem::reset()
{
    setBadge(0);
    setHighlighted(false);
}

void TreeItem::refresh()
{
    emitDataChanged();
}

void TreeItem::blink()
{
    d.blink = !d.blink;
    emitDataChanged();
}

void TreeItem::removeChild(TreeItem* child)
{
    if (d.highlightedChildren.remove(child) && d.highlighted && !isExpanded())
        emitDataChanged();
}
