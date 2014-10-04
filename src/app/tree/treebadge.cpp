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

#include "treebadge.h"
#include <QStyleOptionFrame>
#include <QPainter>
#include <QStyle>
#include <QHash>

TreeBadge::TreeBadge(QWidget* parent) : QLabel(parent)
{
    d.num = 0;
    d.hilite = false;
    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setVisible(false);
}

TreeBadge* TreeBadge::instance(QWidget* parent)
{
    static QHash<QWidget*, TreeBadge*> badges;
    QWidget* window = parent ? parent->window() : 0;
    TreeBadge* badge = badges.value(window);
    if (!badge) {
        badge = new TreeBadge(window);
        badges.insert(window, badge);
    }
    return badge;
}

void TreeBadge::setNum(int num)
{
    d.num = num;
    QString txt;
    if (d.num > 999)
        txt = QLatin1String("...");
    else
        txt = fontMetrics().elidedText(QString::number(d.num), Qt::ElideRight, width());
    setText(txt);
}

void TreeBadge::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    drawBackground(&painter);
    QRect cr = contentsRect();
    cr.adjust(margin(), margin(), -margin(), -margin());
    style()->drawItemText(&painter, cr, alignment(), palette(), isEnabled(), text(), foregroundRole());
}

void TreeBadge::drawBackground(QPainter* painter)
{
    QStyleOptionFrame frame;
    frame.init(this);
    int frameShape  = frameStyle() & QFrame::Shape_Mask;
    int frameShadow = frameStyle() & QFrame::Shadow_Mask;
    frame.frameShape = Shape(int(frame.frameShape) | frameShape);
    frame.rect = frameRect();
    switch (frameShape) {
        case QFrame::Box:
        case QFrame::HLine:
        case QFrame::VLine:
        case QFrame::StyledPanel:
        case QFrame::Panel:
            frame.lineWidth = lineWidth();
            frame.midLineWidth = midLineWidth();
            break;
        default:
            frame.lineWidth = frameWidth();
            break;
    }
    if (frameShadow == Sunken)
        frame.state |= QStyle::State_Sunken;
    else if (frameShadow == Raised)
        frame.state |= QStyle::State_Raised;
    if (d.hilite)
        frame.state |= QStyle::State_On;
    style()->drawPrimitive(QStyle::PE_Widget, &frame, painter, this);
    style()->drawControl(QStyle::CE_ShapedFrame, &frame, painter, this);
}
