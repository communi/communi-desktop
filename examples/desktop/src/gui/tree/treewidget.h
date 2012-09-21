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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>
#include <QColor>
#include <QHash>

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget* parent = 0);

    enum ItemStatus { Active, Inactive, Alert, Highlight };

    QColor statusColor(ItemStatus status) const;
    void setStatusColor(ItemStatus status, const QColor& color);

signals:
    void menuRequested(QTreeWidgetItem* item, const QPoint& pos);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private:
    struct Private
    {
        QHash<ItemStatus, QColor> colors;
    } d;
};

#endif // TREEWIDGET_H
