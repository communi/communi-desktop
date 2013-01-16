/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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
#include "sessiontreewidget.h"
#include "sessiontreeitem.h"
#include <QStyleOptionViewItem>
#include <QApplication>
#include <QMouseEvent>
#include <QLineEdit>
#include <QPalette>
#include <QPainter>

SessionTreeDelegate::SessionTreeDelegate(SessionTreeWidget* parent) : QStyledItemDelegate(parent)
{
    parent->viewport()->installEventFilter(this);
    parent->viewport()->setAttribute(Qt::WA_Hover);
}

QSize SessionTreeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    if (!index.parent().isValid()) {
        static int height = 0;
        if (!height) {
            QLineEdit lineEdit;
            lineEdit.setStyleSheet("QLineEdit { border: 1px solid transparent; }");
            height = lineEdit.sizeHint().height();
        }
        size.setHeight(height);
    }
    return size;
}

void SessionTreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.parent().isValid()) {
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

    if (option.state & QStyle::State_MouseOver) {
        QRect iconRect = QStyle::alignedRect(option.direction, Qt::AlignRight, QSize(option.decorationSize.width(), option.rect.height()), option.rect);
        QRect displayRect = option.rect.adjusted(0, 0, -option.decorationSize.width(), 0);
        const_cast<QStyleOptionViewItem&>(option).rect = QStyle::alignedRect(option.direction, Qt::AlignLeft, displayRect.size(), option.rect);

        if (option.state & QStyle::State_Selected)
            painter->fillRect(iconRect, option.palette.brush(QPalette::Highlight));
        static QIcon icon(":/resources/iconmonstr/close.png");
        icon.paint(painter, iconRect);
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool SessionTreeDelegate::eventFilter(QObject* object, QEvent* event)
{
    SessionTreeWidget* tree = static_cast<SessionTreeWidget*>(parent());
    if (event->type() == QEvent::MouseButtonPress) {
        pressedIndex = QModelIndex();
        QPoint pos = static_cast<QMouseEvent*>(event)->pos();
        QModelIndex index = tree->indexAt(pos);
        if (index.isValid()) {
            QStyleOptionViewItem option = tree->viewOptions();
            option.rect = tree->visualRect(index);
            QRect iconRect = QStyle::alignedRect(option.direction, Qt::AlignRight, QSize(option.decorationSize.width(), option.rect.height()), option.rect);
            if (iconRect.contains(pos))
                pressedIndex = index;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (pressedIndex.isValid()) {
            QPoint pos = static_cast<QMouseEvent*>(event)->pos();
            QModelIndex index = tree->indexAt(pos);
            if (index == pressedIndex) {
                QStyleOptionViewItem option = tree->viewOptions();
                option.rect = tree->visualRect(index);
                QRect iconRect = QStyle::alignedRect(option.direction, Qt::AlignRight, QSize(option.decorationSize.width(), option.rect.height()), option.rect);
                if (iconRect.contains(pos))
                    emit closeRequested(static_cast<SessionTreeItem*>(tree->itemFromIndex(index)));
            }
            pressedIndex = QModelIndex();
        }
    }
    return QStyledItemDelegate::eventFilter(object, event);
}
