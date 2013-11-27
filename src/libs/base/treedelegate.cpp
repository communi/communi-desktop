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
#include <QPalette>
#include <QPainter>
#include <QPointer>
#include <QStyle>
#include <QColor>

class TreeHeader : public QWidget
{
    Q_OBJECT

public:
    TreeHeader(QWidget* parent = 0) : QWidget(parent) { d.state = QStyle::State_None; }

    void setText(const QString& text) { d.text = text; }
    void setIcon(const QIcon& icon) { d.icon = icon; }
    void setState(QStyle::State state) { d.state = state; }

protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOptionHeader option;
        option.init(this);
        option.state = (d.state | QStyle::State_Raised | QStyle::State_Horizontal);
        if (d.state & QStyle::State_Selected)
            option.state |= QStyle::State_On;
        option.icon = d.icon;
        option.text = d.text;
        option.position = QStyleOptionHeader::OnlyOneSection;
        QStylePainter painter(this);
        painter.drawControl(QStyle::CE_Header, option);
    }

private:
    struct Private {
        QIcon icon;
        QString text;
        QStyle::State state;
    } d;
};

class TreeBadge : public QWidget
{
    Q_OBJECT

public:
    TreeBadge(QWidget* parent = 0) : QWidget(parent) { }

protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOption option;
        option.init(this);
        QStylePainter painter(this);
        painter.drawPrimitive(QStyle::PE_Widget, option);
    }
};

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.parent().isValid()) {
        static QPointer<TreeHeader> header;
        if (!header)
            header = new TreeHeader(const_cast<QWidget*>(option.widget));

        QPalette pal = option.palette;
        QVariant fg = index.data(Qt::ForegroundRole);
        if (fg.isValid()) {
            pal.setColor(QPalette::Text, fg.value<QColor>());
            pal.setColor(QPalette::ButtonText, fg.value<QColor>());
            pal.setColor(QPalette::WindowText, fg.value<QColor>());
        }
        header->setPalette(pal);
        header->setText(index.data(Qt::DisplayRole).toString());
        header->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
        header->setState(option.state);
        header->setGeometry(option.rect);
        painter->translate(option.rect.topLeft());
        header->render(painter);
        painter->translate(-option.rect.topLeft());
    } else {
        int num = 0;
        if (index.parent().isValid())
            num = index.data(TreeRole::Badge).toInt();

        if (num > 0) {
            QRect rect;
            rect.setWidth(option.rect.width() - 2);
            const int ascent = option.fontMetrics.ascent();
            rect.setHeight(ascent + qMax(option.rect.height() % 2, ascent % 2));
            rect.moveCenter(option.rect.center());

            QBrush brush = QPalette().color(QPalette::Mid); // Theme::instance()->badge(Theme::Background);
            QVariant bg = index.data(Qt::BackgroundRole);
            if (bg.isValid())
                brush = bg.value<QBrush>();

            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(brush);
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawRoundedRect(rect, 40, 80, Qt::RelativeSize);
            painter->restore();
        }

        QStyledItemDelegate::paint(painter, option, index);

        if (num > 0) {
            painter->save();
            QFont font;
            if (font.pointSize() != -1)
                font.setPointSizeF(0.8 * font.pointSizeF());
            painter->setFont(font);

            QString txt;
            if (num > 999)
                txt = QLatin1String("...");
            else
                txt = QFontMetrics(font).elidedText(QString::number(num), Qt::ElideRight, option.rect.width());

            painter->setPen(QPalette().color(QPalette::Midlight));
            painter->drawText(option.rect, Qt::AlignCenter, txt);
            painter->restore();
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
