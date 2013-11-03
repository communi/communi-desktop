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
#include <QLineEdit>
#include <QPalette>
#include <QPainter>

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QSize TreeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
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

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.parent().isValid()) {
        const bool selected = option.state & QStyle::State_Selected;
        const_cast<QStyleOptionViewItem&>(option).state &= ~(QStyle::State_Selected | QStyle::State_MouseOver);

        QColor c1 = option.palette.color(QPalette::Light);
        QColor c2 = option.palette.color(QPalette::Button);
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
        painter->setPen(option.palette.color(QPalette::Dark));
        painter->drawLines(lines);
        painter->setPen(oldPen);
    }

    QStyledItemDelegate::paint(painter, option, index);

    if (index.column() == 1) {
        int badge = index.data(TreeRole::Badge).toInt();
        if (badge > 0) {
            QRect rect;
            rect.setWidth(option.rect.width() - 2);
            const int ascent = option.fontMetrics.ascent();
            rect.setHeight(ascent + qMax(option.rect.height() % 2, ascent % 2));
            rect.moveCenter(option.rect.center());

            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(option.palette.color(QPalette::Button));
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawRoundedRect(rect, 40, 80, Qt::RelativeSize);

            QFont font;
            if (font.pointSize() != -1)
                font.setPointSizeF(0.8 * font.pointSizeF());
            painter->setFont(font);

            QString txt;
            if (badge > 999)
                txt = QLatin1String("...");
            else
                txt = option.fontMetrics.elidedText(QString::number(badge), Qt::ElideRight, option.rect.width());

            QColor color = option.palette.color(QPalette::ButtonText);
            color.setAlpha(128);
            painter->setPen(color);
            painter->drawText(option.rect, Qt::AlignCenter, txt);
            painter->restore();
        }
    }
}
