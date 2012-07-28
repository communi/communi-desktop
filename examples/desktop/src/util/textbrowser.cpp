/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

#include "textbrowser.h"
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

TextBrowser::TextBrowser(QWidget* parent) : QTextBrowser(parent), ub(-1)
{
}

int TextBrowser::unseenBlock() const
{
    return ub;
}

void TextBrowser::setUnseenBlock(int block)
{
    ub = block;
}

void TextBrowser::resizeEvent(QResizeEvent* event)
{
    QTextBrowser::resizeEvent(event);

    // http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=240940
    QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
}

void TextBrowser::scrollToTop()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
}

void TextBrowser::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

void TextBrowser::scrollToNextPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void TextBrowser::scrollToPreviousPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void TextBrowser::paintEvent(QPaintEvent* event)
{
    QTextBrowser::paintEvent(event);

    QTextBlock block;
    if (ub > 0)
        block = document()->findBlockByNumber(ub);

    if (block.isValid())
    {
        QPainter painter(viewport());
        painter.setPen(Qt::DashLine);
        painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());

        QRectF br = document()->documentLayout()->blockBoundingRect(block);
        painter.drawLine(br.topLeft(), br.topRight());
    }
}
