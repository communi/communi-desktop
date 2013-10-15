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

#include "treedelegate.h"
#include "treerole.h"
#include <QStyleOptionViewItem>

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
//    const bool highlighted = index.data(HighlightRole).toBool();
//    if (highlighted && !(option.state & QStyle::State_Selected))
//        const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_Active;
//    else
//        const_cast<QStyleOptionViewItem&>(option).state &= ~QStyle::State_Active;
    QStyledItemDelegate::paint(painter, option, index);
}
