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
#include <QStyleOptionViewItem>
#include <QPalette>
#include <QPainter>

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.parent().isValid()) {
        const bool selected = option.state & QStyle::State_Selected;
        const_cast<QStyleOptionViewItem&>(option).state &= ~(QStyle::State_Selected | QStyle::State_MouseOver);

        QPalette pal; // TODO
        QColor c1 = pal.color(QPalette::Light);
        QColor c2 = pal.color(QPalette::Button);
        if (selected)
            qSwap(c1, c2);

        QLinearGradient gradient(option.rect.topLeft(), option.rect.bottomLeft());
        gradient.setColorAt(0.0, c1);
        gradient.setColorAt(1.0, c2);
        painter->fillRect(option.rect, gradient);

        QVector<QLine> lines;
        if (index.row() > 0)
            lines += QLine(option.rect.topLeft(), option.rect.topRight());
        lines += QLine(option.rect.bottomLeft(), option.rect.bottomRight());
        QPen oldPen = painter->pen();
        painter->setPen(pal.color(QPalette::Dark));
        painter->drawLines(lines);
        painter->setPen(oldPen);
    }

    QStyledItemDelegate::paint(painter, option, index);
}
