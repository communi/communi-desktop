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
        if (d.inactive)
            return treeWidget()->palette().color(QPalette::Disabled, QPalette::Highlight);
        if (d.alerted)
            return QColor(Qt::red); // TODO: treeWidget()->palette().color(QPalette::Highlight);
        if (d.highlighted)
            return treeWidget()->palette().color(QPalette::Highlight);
        return treeWidget()->palette().color(QPalette::WindowText);
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
