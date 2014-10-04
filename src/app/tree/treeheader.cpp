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

#include "treeheader.h"
#include <QStyleOptionHeader>
#include <QStylePainter>
#include <QHash>

TreeHeader::TreeHeader(QWidget* parent) : QFrame(parent)
{
    d.state = QStyle::State_None;
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setVisible(false);
}

TreeHeader* TreeHeader::instance(QWidget* parent)
{
    static QHash<QWidget*, TreeHeader*> headers;
    QWidget* window = parent ? parent->window() : 0;
    TreeHeader* header = headers.value(window);
    if (!header) {
        header = new TreeHeader(window);
        headers.insert(window, header);
    }
    return header;
}

void TreeHeader::paintEvent(QPaintEvent*)
{
    QStyleOptionHeader option;
    option.init(this);
#ifdef Q_OS_WIN
    option.rect.adjust(0, 0, 0, 1);
#endif
    option.state = (d.state | QStyle::State_Raised | QStyle::State_Horizontal);
    option.icon = d.icon;
    option.text = d.text;
    option.position = QStyleOptionHeader::OnlyOneSection;
    QStylePainter painter(this);
    painter.drawControl(QStyle::CE_Header, option);
}
