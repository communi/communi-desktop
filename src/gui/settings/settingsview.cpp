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

#include "settingsview.h"
#include "settingsdelegate.h"
#include <QSortFilterProxyModel>
#include <QHeaderView>

SettingsView::SettingsView(QWidget* parent) : QTreeView(parent)
{
    setIndentation(0);
    setSortingEnabled(true);
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setItemDelegate(new SettingsDelegate(this));

    proxy = new QSortFilterProxyModel(this);
    proxy->setDynamicSortFilter(true);
    QTreeView::setModel(proxy);
}

void SettingsView::setModel(QAbstractItemModel* model)
{
    proxy->setSourceModel(model);
    sortByColumn(0, Qt::AscendingOrder);
    if (header()->count() > 0)
        header()->setResizeMode(0, QHeaderView::ResizeToContents);
}
