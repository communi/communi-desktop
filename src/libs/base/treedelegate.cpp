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
#include <QStyleOptionHeader>
#include <QStylePainter>
#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QPointer>
#include <QStyle>
#include <QColor>

class TreeHeader : public QWidget
{
    Q_OBJECT

public:
    TreeHeader(QWidget* parent = 0) : QWidget(parent) { d.highlighted = false; d.state = QStyle::State_None; }

    void setText(const QString& text) { d.text = text; }
    void setIcon(const QIcon& icon) { d.icon = icon; }
    void setState(QStyle::State state) { d.state = state; }
    void setHighlighted(bool highlighted) { d.highlighted = highlighted; }

protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOptionHeader option;
        option.init(this);
#ifdef Q_OS_WIN
        option.rect.adjust(0, 0, 0, 1);
#endif
        option.state = (d.state | QStyle::State_Raised | QStyle::State_Horizontal);
        if (d.state & QStyle::State_Selected)
            option.state |= QStyle::State_On;
        option.icon = d.icon;
        option.text = d.text;
        option.position = QStyleOptionHeader::OnlyOneSection;
        if (d.highlighted) {
            option.palette.setColor(QPalette::Text, option.palette.color(QPalette::HighlightedText));
            option.palette.setColor(QPalette::ButtonText, option.palette.color(QPalette::HighlightedText));
            option.palette.setColor(QPalette::WindowText, option.palette.color(QPalette::HighlightedText));
        }
        QStylePainter painter(this);
        painter.drawControl(QStyle::CE_Header, option);
    }

private:
    struct Private {
        QIcon icon;
        QString text;
        bool highlighted;
        QStyle::State state;
    } d;
};

class TreeBadge : public QWidget
{
    Q_OBJECT

public:
    TreeBadge(QWidget* parent = 0) : QWidget(parent) { d.num = 0; d.hilite = false; }

    void setNum(int num) { d.num = num; }
    void setHighlighted(int hilite) { d.hilite = hilite; }

protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOption option;
        option.init(this);
        if (d.hilite)
            option.state |= QStyle::State_On;
        QStylePainter painter(this);
        painter.drawPrimitive(QStyle::PE_Widget, option);

        QString txt;
        if (d.num > 999)
            txt = QLatin1String("...");
        else
            txt = option.fontMetrics.elidedText(QString::number(d.num), Qt::ElideRight, option.rect.width());
        painter.drawText(option.rect, Qt::AlignCenter, txt);
    }

private:
    struct Private {
        int num;
        bool hilite;
    } d;
};

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    bool active = index.data(TreeRole::Active).toBool();
    bool hilite = index.data(TreeRole::Highlight).toBool();

    if (!index.parent().isValid()) {
        static QPointer<TreeHeader> header;
        if (!header) {
            header = new TreeHeader(const_cast<QWidget*>(option.widget));
            header->setVisible(false);
        }

        header->setEnabled(active);
        header->setHighlighted(hilite);
        header->setText(index.data(Qt::DisplayRole).toString());
        header->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
        header->setState(option.state);
        header->setGeometry(option.rect);
        painter->translate(option.rect.topLeft());
        header->render(painter);
        painter->translate(-option.rect.topLeft());
    } else {
        static QPointer<TreeBadge> badge;
        if (!badge) {
            badge = new TreeBadge(const_cast<QWidget*>(option.widget));
            badge->setAttribute(Qt::WA_TranslucentBackground);
            badge->setAttribute(Qt::WA_NoSystemBackground);
            badge->setVisible(false);
        }

        if (hilite)
            const_cast<QStyleOptionViewItem&>(option).palette.setColor(QPalette::Text, QApplication::palette("TreeItem").color(QPalette::HighlightedText));
        else
            const_cast<QStyleOptionViewItem&>(option).palette.setColor(QPalette::Text, QApplication::palette("TreeItem").color(QPalette::Text));
        if (!active)
            const_cast<QStyleOptionViewItem&>(option).palette.setColor(QPalette::Text, option.palette.color(QPalette::Disabled, QPalette::Text));

        QStyledItemDelegate::paint(painter, option, index);

        int num = index.data(TreeRole::Badge).toInt();
        if (num > 0) {
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
    if (index.parent().isValid()) {
        QStyleOptionViewItemV4* v4 = qstyleoption_cast<QStyleOptionViewItemV4*>(option);
        if (v4)
            v4->backgroundBrush = Qt::transparent;
    }
}

#include "treedelegate.moc"
