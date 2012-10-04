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

#include "sessiontreedelegate.h"
#include <QStyleOptionViewItem>
#include <QApplication>
#include <QPalette>
#include <QPainter>

SessionTreeDelegate::SessionTreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void SessionTreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const bool topLevel = !index.parent().isValid();
    if (topLevel)
    {
        const bool selected = option.state & QStyle::State_Selected;
        const_cast<QStyleOptionViewItem&>(option).state &= ~QStyle::State_Selected;

        QColor c1 = qApp->palette().color(QPalette::Light);
        QColor c2 = qApp->palette().color(QPalette::Button);
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
        painter->setPen(qApp->palette().color(QPalette::Dark));
        painter->drawLines(lines);
    }

    QStyledItemDelegate::paint(painter, option, index);
}
