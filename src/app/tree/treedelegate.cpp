/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "treedelegate.h"
#include "treewidget.h"
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
    d.transient = false;
}

bool TreeDelegate::isTransient() const
{
    return d.transient;
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
    bool active = index.data(TreeRole::Active).toBool();
    if (!active)
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
        bool hilite = index.data(TreeRole::Highlight).toBool();
        if (hilite)
            const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_On;

        // a selected inactive buffer should be visually the same then
        // a selected and disabled tree item. however, a disabled tree
        // item cannot have selection with all Qt styles, so we do the
        // drawing in two steps. first the selection without text, and
        // then disabled text without selection

        d.transient = !active && option.state & QStyle::State_Selected;

        QStyledItemDelegate::paint(painter, option, index);

        if (d.transient) {
            d.transient = false;
            const_cast<QStyleOptionViewItem&>(option).state &= ~(QStyle::State_Enabled | QStyle::State_Selected);
            QStyledItemDelegate::paint(painter, option, index);
        }

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
