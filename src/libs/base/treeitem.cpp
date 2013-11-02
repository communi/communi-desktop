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

TreeItem::TreeItem(TreeItem* parent) : QTreeWidgetItem(parent)
{
}

TreeItem::TreeItem(TreeWidget* parent) : QTreeWidgetItem(parent)
{
}

TreeItem* TreeItem::parentItem() const
{
    return static_cast<TreeItem*>(QTreeWidgetItem::parent());
}

TreeWidget* TreeItem::treeWidget() const
{
    return static_cast<TreeWidget*>(QTreeWidgetItem::treeWidget());
}
