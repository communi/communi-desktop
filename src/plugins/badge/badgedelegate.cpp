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

#include "badgedelegate.h"
#include "treerole.h"
#include <QStyleOptionViewItem>
#include <QPalette>
#include <QPainter>

BadgeDelegate::BadgeDelegate(QObject* parent) : TreeDelegate(parent)
{
}

void BadgeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int badge = 0;
    if (index.parent().isValid())
        badge = index.data(TreeRole::Badge).toInt();

    if (badge > 0) {
        QRect rect;
        rect.setWidth(option.rect.width() - 2);
        const int ascent = option.fontMetrics.ascent();
        rect.setHeight(ascent + qMax(option.rect.height() % 2, ascent % 2));
        rect.moveCenter(option.rect.center());

        QColor color = index.data(Qt::BackgroundRole).value<QColor>();
        if (!color.isValid())
            color = QPalette().color(QPalette::Mid);

        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(rect, 40, 80, Qt::RelativeSize);
        painter->restore();
    }

    TreeDelegate::paint(painter, option, index);

    if (badge > 0) {
        painter->save();
        QFont font;
        if (font.pointSize() != -1)
            font.setPointSizeF(0.8 * font.pointSizeF());
        painter->setFont(font);

        QString txt;
        if (badge > 999)
            txt = QLatin1String("...");
        else
            txt = QFontMetrics(font).elidedText(QString::number(badge), Qt::ElideRight, option.rect.width());

        painter->setPen(QPalette().color(QPalette::Midlight)); // TODO
        painter->drawText(option.rect, Qt::AlignCenter, txt);
        painter->restore();
    }
}

void BadgeDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    if (index.parent().isValid()) {
        QStyleOptionViewItemV4* v4 = qstyleoption_cast<QStyleOptionViewItemV4*>(option);
        if (v4)
            v4->backgroundBrush = Qt::transparent;
    }
}
