/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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

SessionTreeItem::SessionTreeItem(Session* session, QTreeWidget* parent) : QTreeWidgetItem(parent)
{
    d.session = session;
    d.alerted = false;
    d.inactive = true;
    d.highlighted = false;
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled);
}

SessionTreeItem::SessionTreeItem(Session* session, QTreeWidgetItem* parent) : QTreeWidgetItem(parent)
{
    d.session = session;
    d.alerted = false;
    d.inactive = false;
    d.highlighted = false;
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

SessionTreeItem::~SessionTreeItem()
{
    if (SessionTreeItem* p = static_cast<SessionTreeItem*>(parent())) {
        p->d.alertedChildren.remove(this);
        p->d.highlightedChildren.remove(this);
    }

    if (SessionTreeWidget* tw = static_cast<SessionTreeWidget*>(treeWidget())) {
        tw->d.alertedItems.remove(this);
        tw->d.resetedItems.remove(this);
    }
}

Session* SessionTreeItem::session() const
{
    return d.session;
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
        if (!parent() && d.inactive)
            return tw->statusColor(SessionTreeWidget::Inactive);
        if (d.alerted || (!isExpanded() && !d.alertedChildren.isEmpty()))
            return tw->currentAlertColor();
        if (d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty()))
            return tw->statusColor(SessionTreeWidget::Highlight);
        return tw->statusColor(d.inactive ? SessionTreeWidget::Inactive : SessionTreeWidget::Active);
    }
    return QTreeWidgetItem::data(column, role);
}

bool SessionTreeItem::isAlerted() const
{
    return d.alerted;
}

void SessionTreeItem::setAlerted(bool alerted)
{
    if (d.alerted != alerted) {
        d.alerted = alerted;
        if (SessionTreeItem* p = static_cast<SessionTreeItem*>(parent())) {
            if (alerted)
                p->d.alertedChildren.insert(this);
            else
                p->d.alertedChildren.remove(this);
            if (!p->isExpanded())
                p->emitDataChanged();
        }
        emitDataChanged();
    }
}

bool SessionTreeItem::isInactive() const
{
    return d.inactive;
}

void SessionTreeItem::setInactive(bool inactive)
{
    if (d.inactive != inactive) {
        d.inactive = inactive;
        emitDataChanged();
    }
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
    QVariantHash state = static_cast<SessionTreeWidget*>(treeWidget())->d.state;
    QStringList receivers = state.value(parent()->text(0)).toStringList();
    const int a = receivers.indexOf(text(0));
    const int b = receivers.indexOf(other.text(0));
    if (a == -1 && b != -1)
        return false;
    if (a != -1 && b == -1)
        return true;
    return a < b;
}
