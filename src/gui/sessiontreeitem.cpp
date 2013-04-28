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

#include "sessiontreeitem.h"
#include "sessiontreewidget.h"
#include "messageview.h"

SessionTreeItem::SessionTreeItem(MessageView* view, QTreeWidget* parent) : QTreeWidgetItem(parent)
{
    d.view = view;
    d.highlighted = false;
    setText(0, view->receiver());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled);
}

SessionTreeItem::SessionTreeItem(MessageView* view, QTreeWidgetItem* parent) : QTreeWidgetItem(parent)
{
    d.view = view;
    d.highlighted = false;
    setText(0, view->receiver());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

SessionTreeItem::~SessionTreeItem()
{
}

Session* SessionTreeItem::session() const
{
    return d.view->session();
}

MessageView* SessionTreeItem::view() const
{
    return d.view;
}

SessionTreeItem* SessionTreeItem::findChild(const QString& name) const
{
    for (int i = 0; i < childCount(); ++i)
        if (child(i)->text(0).compare(name, Qt::CaseInsensitive) == 0)
            return static_cast<SessionTreeItem*>(child(i));
    return 0;
}

QVariant SessionTreeItem::data(int column, int role) const
{
    if (role == Qt::ForegroundRole) {
        SessionTreeWidget* tw = static_cast<SessionTreeWidget*>(treeWidget());
        if (!d.view->isActive())
            return tw->statusColor(SessionTreeWidget::Inactive);
        if (d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty()))
            return tw->currentHighlightColor();
        return tw->statusColor(SessionTreeWidget::Active);
    } else if (role == BadgeColorRole) {
        SessionTreeWidget* tw = static_cast<SessionTreeWidget*>(treeWidget());
        if (d.view->isActive() && d.highlighted)
            return tw->currentBadgeColor();
        return qApp->palette().color(QPalette::Dark);
    } else if (role == HighlightRole) {
        return d.highlighted;
    }
    return QTreeWidgetItem::data(column, role);
}

int SessionTreeItem::badge() const
{
    return data(1, BadgeRole).toInt();
}

void SessionTreeItem::setBadge(int badge)
{
    setData(1, BadgeRole, badge);
}

bool SessionTreeItem::isHighlighted() const
{
    return d.highlighted;
}

void SessionTreeItem::setHighlighted(bool highlighted)
{
    if (d.highlighted != highlighted) {
        d.highlighted = highlighted;
        if (SessionTreeItem* p = static_cast<SessionTreeItem*>(parent())) {
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

bool SessionTreeItem::operator<(const QTreeWidgetItem& other) const
{
    Q_ASSERT(parent() && other.parent());
    QStringList sortOrder = static_cast<SessionTreeItem*>(parent())->d.sortOrder;
    const int a = sortOrder.indexOf(text(0));
    const int b = sortOrder.indexOf(other.text(0));
    if (a == -1 && b != -1)
        return false;
    if (a != -1 && b == -1)
        return true;
    return a < b;
}
