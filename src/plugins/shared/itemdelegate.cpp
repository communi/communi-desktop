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

#include "itemdelegate.h"

ItemDelegate::ItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
    d.height = 0;
}

int ItemDelegate::itemHeight() const
{
    return d.height;
}

void ItemDelegate::setItemHeight(int height)
{
    d.height = height;
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    if (d.height > 0)
        size.setHeight(d.height);
    return size;
}
