/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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
}

SessionTreeItem::SessionTreeItem(Session* session, QTreeWidgetItem* parent) : QTreeWidgetItem(parent)
{
    d.session = session;
    d.alerted = false;
    d.inactive = false;
    d.highlighted = false;
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
    if (role == Qt::ForegroundRole)
    {
        SessionTreeWidget* tw = static_cast<SessionTreeWidget*>(treeWidget());
        if (d.inactive)
            return tw->statusColor(SessionTreeWidget::Inactive);
        if (d.alerted)
            return tw->statusColor(SessionTreeWidget::Alert);
        if (d.highlighted)
            return tw->statusColor(SessionTreeWidget::Highlight);
        return tw->statusColor(SessionTreeWidget::Active);
    }
    return QTreeWidgetItem::data(column, role);
}

void SessionTreeItem::setAlerted(bool alerted)
{
    d.alerted = alerted;
    emitDataChanged();
}

void SessionTreeItem::setInactive(bool inactive)
{
    d.inactive = inactive;
    emitDataChanged();
}

void SessionTreeItem::setHighlighted(bool highlighted)
{
    d.highlighted = highlighted;
    emitDataChanged();
}
