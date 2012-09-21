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
#include "treewidget.h"

SessionTreeItem::SessionTreeItem(QTreeWidget* parent) : QTreeWidgetItem(parent)
{
    d.alerted = false;
    d.inactive = true;
    d.highlighted = false;
}

QVariant SessionTreeItem::data(int column, int role) const
{
    if (role == Qt::ForegroundRole)
    {
        TreeWidget* tw = static_cast<TreeWidget*>(treeWidget());
        if (d.inactive)
            return tw->statusColor(TreeWidget::Inactive);
        if (d.alerted)
            return tw->statusColor(TreeWidget::Alert);
        if (d.highlighted)
            return tw->statusColor(TreeWidget::Highlight);
        return tw->statusColor(TreeWidget::Active);
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
