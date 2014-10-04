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

#include "treeindicator.h"
#include <QStyleOptionFrame>
#include <QStylePainter>
#include <QStyle>
#include <QHash>
#include <qmath.h>

TreeIndicator::TreeIndicator(QWidget* parent) : QFrame(parent)
{
    d.lag = 0;
    d.hilite = false;
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setVisible(false);
}

TreeIndicator* TreeIndicator::instance(QWidget* parent)
{
    static QHash<QWidget*, TreeIndicator*> indicators;
    QWidget* window = parent ? parent->window() : 0;
    TreeIndicator* indicator = indicators.value(window);
    if (!indicator) {
        indicator = new TreeIndicator(window);
        indicators.insert(window, indicator);
    }
    return indicator;
}

void TreeIndicator::paintEvent(QPaintEvent*)
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

    if (d.lag > 0 && !d.hilite) {
        qreal f = qMin(100.0, qSqrt(d.lag)) / 100;
        QColor color = QColor::fromHsl(120 - f * 120, 96, 152); // TODO
        setStyleSheet(QString("background-color:%1").arg(color.name()));
    } else {
        setStyleSheet(QString());
    }

    QStylePainter painter(this);
    painter.drawPrimitive(QStyle::PE_Widget, frame);
    painter.drawControl(QStyle::CE_ShapedFrame, frame);
}
