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

#include "listdelegate.h"
#include <QLineEdit>

ListDelegate::ListDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QSize ListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    static int height = 0;
    if (!height) {
        QLineEdit lineEdit;
        lineEdit.setStyleSheet("QLineEdit { border: 1px solid transparent; }");
        height = lineEdit.sizeHint().height();
    }
    size.setHeight(height);
    return size;
}
