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

void TreeBadge::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    drawBackground(&painter);
    painter.end();
    QLabel::paintEvent(event);
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
