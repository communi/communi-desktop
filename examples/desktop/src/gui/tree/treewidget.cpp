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

#include "treewidget.h"
#include <QContextMenuEvent>

TreeWidget::TreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    setAnimated(true);
    setHeaderHidden(true);

    d.colors[Active] = palette().color(QPalette::WindowText);
    d.colors[Inactive] = palette().color(QPalette::Disabled, QPalette::Highlight);
    d.colors[Alert] = QColor(Qt::red); //palette().color(QPalette::Highlight);
    d.colors[Highlight] = QColor(Qt::green); //palette().color(QPalette::Highlight);
}

QColor TreeWidget::statusColor(TreeWidget::ItemStatus status) const
{
    return d.colors.value(status);
}

void TreeWidget::setStatusColor(TreeWidget::ItemStatus status, const QColor& color)
{
    d.colors[status] = color;
}

void TreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if (item)
        emit menuRequested(item, event->globalPos());
}
