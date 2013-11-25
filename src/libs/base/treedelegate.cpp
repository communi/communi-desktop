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
#include <QStyleOptionHeader>
#include <QStylePainter>
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
        QStyledItemDelegate::paint(painter, option, index);
    }
}

#include "treedelegate.moc"
