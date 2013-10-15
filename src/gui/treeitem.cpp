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
#include "itemdelegate.h"
#include <IrcBuffer>

TreeItem::TreeItem(IrcBuffer* buffer, QTreeWidget* parent) : QTreeWidgetItem(parent)
{
    d.buffer = buffer;
    d.highlighted = false;
    d.sortOrder = Manual;
    setText(0, buffer->title());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
}

TreeItem::TreeItem(IrcBuffer* buffer, QTreeWidgetItem* parent) : QTreeWidgetItem(parent)
{
    d.buffer = buffer;
    d.highlighted = false;
    d.sortOrder = Manual;
    setText(0, buffer->title());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

TreeItem::~TreeItem()
{
}

IrcBuffer* TreeItem::buffer() const
{
    return d.buffer;
}

IrcConnection* TreeItem::connection() const
{
    return d.buffer->connection();
}

TreeItem* TreeItem::findChild(const QString& name) const
{
    for (int i = 0; i < childCount(); ++i)
        if (child(i)->text(0).compare(name, Qt::CaseInsensitive) == 0)
            return static_cast<TreeItem*>(child(i));
    return 0;
}

QVariant TreeItem::data(int column, int role) const
{
    if (role == Qt::ForegroundRole) {
        TreeWidget* tw = static_cast<TreeWidget*>(treeWidget());
        if (!d.buffer->isActive())
            return tw->statusColor(TreeWidget::Inactive);
        if (d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty()))
            return tw->currentHighlightColor();
        return tw->statusColor(TreeWidget::Active);
    } else if (role == ItemDelegate::BadgeColorRole) {
        TreeWidget* tw = static_cast<TreeWidget*>(treeWidget());
        if (!isSelected() && d.buffer->isActive() && d.highlighted)
            return tw->currentBadgeColor();
        return tw->statusColor(TreeWidget::Badge);
    } else if (role == ItemDelegate::HighlightRole) {
        return d.highlighted;
    }
    return QTreeWidgetItem::data(column, role);
}

int TreeItem::badge() const
{
    return data(1, ItemDelegate::BadgeRole).toInt();
}

void TreeItem::setBadge(int badge)
{
    setData(1, ItemDelegate::BadgeRole, badge);
}

bool TreeItem::isHighlighted() const
{
    return d.highlighted;
}

void TreeItem::setHighlighted(bool highlighted)
{
    if (d.highlighted != highlighted) {
        d.highlighted = highlighted;
        if (TreeItem* p = static_cast<TreeItem*>(parent())) {
            if (highlighted)
                p->d.highlightedChildren.insert(this);
            else
                p->d.highlightedChildren.remove(this);
            if (!p->isExpanded())
                p->emitDataChanged();
        }
        emitDataChanged();
    }
}

void TreeItem::sort(SortOrder order)
{
    if (d.sortOrder != order) {
        if (d.sortOrder == Manual)
            resetManualSortOrder();
        d.sortOrder = order;
        sortChildren(0, Qt::AscendingOrder);
    }
}

TreeItem::SortOrder TreeItem::currentSortOrder() const
{
    if (const TreeItem* p = static_cast<const TreeItem*>(parent()))
        return p->currentSortOrder();
    return d.sortOrder;
}

QStringList TreeItem::manualSortOrder() const
{
    if (const TreeItem* p = static_cast<const TreeItem*>(parent()))
        return p->manualSortOrder();
    return d.manualOrder;
}

void TreeItem::setManualSortOrder(const QStringList& order)
{
    if (d.manualOrder != order) {
        d.manualOrder = order;
        sortChildren(0, Qt::AscendingOrder);
    }
}

void TreeItem::resetManualSortOrder()
{
    d.manualOrder.clear();
    for (int i = 0; i < childCount(); ++i)
        d.manualOrder += child(i)->text(0);
}

bool TreeItem::operator<(const QTreeWidgetItem& other) const
{
    Q_ASSERT(parent() && other.parent());
    if (currentSortOrder() == Alphabetic) {
        const TreeItem* otherItem = static_cast<const TreeItem*>(&other);
        const bool a = d.buffer->isChannel();
        const bool b = otherItem->d.buffer->isChannel();
        if (a != b)
            return a;
        return d.buffer->name().localeAwareCompare(otherItem->d.buffer->name()) < 0;
    } else {
        // manual sorting via dnd
        const QStringList sortOrder = manualSortOrder();
        const int a = sortOrder.indexOf(text(0));
        const int b = sortOrder.indexOf(other.text(0));
        if (a == -1 && b != -1)
            return false;
        if (a != -1 && b == -1)
            return true;
        return a < b;
    }
}
