/*
* Copyright (C) 2008-2014 The Communi Project
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
#include "treeheader.h"
#include "treebadge.h"
#include "treerole.h"
#include <QStyleOptionViewItem>
#include <QStylePainter>
#include <QApplication>
#include <QHeaderView>
#include <QTreeView>
#include <QPalette>
#include <QPainter>
#include <QPointer>
#include <QLabel>
#include <QStyle>
#include <QColor>

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QSize TreeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    if (!index.parent().isValid()) {
        // QMacStyle wants a QHeaderView that is a child of QTreeView :/
        QTreeView tree;
        QStyleOptionHeader opt;
        QSize ss = qApp->style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), tree.header());
        TreeHeader* header = TreeHeader::instance(const_cast<QWidget*>(option.widget));
        if (header->minimumSize().isValid())
            ss = ss.expandedTo(header->minimumSize());
        if (header->maximumSize().isValid())
            ss = ss.boundedTo(header->maximumSize());
        if (ss.isValid())
            return ss;
    }
    return sz;
}

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    bool hilite = index.data(TreeRole::Highlight).toBool();
    bool active = index.data(TreeRole::Active).toBool();
    if (!hilite && !active)
        const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_Off;

    if (!index.parent().isValid()) {
        TreeHeader* header = TreeHeader::instance(const_cast<QWidget*>(option.widget));
        header->setText(index.data(Qt::DisplayRole).toString());
        header->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
        header->setState(option.state);
        header->setGeometry(option.rect);
        painter->translate(option.rect.topLeft());
        header->render(painter);
        painter->translate(-option.rect.topLeft());
    } else {
        if (hilite)
            const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_On;

        QStyledItemDelegate::paint(painter, option, index);

        int num = index.data(TreeRole::Badge).toInt();
        if (num > 0) {
            TreeBadge* badge = TreeBadge::instance(const_cast<QWidget*>(option.widget));
            badge->setNum(num);
            badge->setHighlighted(hilite);

            badge->setGeometry(option.rect);
            painter->translate(option.rect.topLeft());
            badge->render(painter);
            painter->translate(-option.rect.topLeft());
        }
    }
}

void TreeDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    if (index.parent().isValid())
        option->backgroundBrush = Qt::transparent;
}
