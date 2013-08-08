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

#include "textbrowser.h"
#include "styledscrollbar.h"
#include <QApplication>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <qmath.h>

TextBrowser::TextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    d.ub = -1;
    d.bud = 0;

    setVerticalScrollBar(new StyledScrollBar(this));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QWidget* TextBrowser::buddy() const
{
    return d.bud;
}

void TextBrowser::setBuddy(QWidget* buddy)
{
    d.bud = buddy;
}

void TextBrowser::addMarker(int block)
{
    d.markers.append(block);
    update();
}

void TextBrowser::removeMarker(int block)
{
    if (d.markers.removeOne(block))
        update();
}

QColor TextBrowser::highlightColor() const
{
    return d.highlightColor;
}

void TextBrowser::setHighlightColor(const QColor& color)
{
    if (d.highlightColor != color) {
        d.highlightColor = color;
        update();
    }
}

QColor TextBrowser::markerColor() const
{
    return d.markerColor;
}

void TextBrowser::setMarkerColor(const QColor& color)
{
    if (d.markerColor != color) {
        d.markerColor = color;
        update();
    }
}

QColor TextBrowser::shadowColor() const
{
    return d.shadowColor;
}

void TextBrowser::setShadowColor(const QColor& color)
{
    if (d.shadowColor != color) {
        d.shadowColor = color;
        update();
    }
}

void TextBrowser::append(const QString& text, bool highlight)
{
    if (!text.isEmpty()) {

        QTextBrowser::append(text);

#if QT_VERSION >= 0x040800
        QTextCursor cursor(document());
        cursor.movePosition(QTextCursor::End);
        QTextBlockFormat format = cursor.blockFormat();
        format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
        cursor.setBlockFormat(format);
#endif // QT_VERSION

        int count = document()->blockCount();

        if (count == document()->maximumBlockCount()) {
            if (d.ub != -1)
                --d.ub;

            QMutableListIterator<int> mt(d.markers);
            while (mt.hasNext()) {
                --mt.next();
                if (mt.value() < 0)
                    mt.remove();
            }

            QMutableListIterator<int> ht(d.highlights);
            while (ht.hasNext()) {
                --ht.next();
                if (ht.value() < 0)
                    ht.remove();
            }
        }

        if (!isVisible() && d.ub == -1)
            d.ub = count - 1;

        if (highlight)
            d.highlights.append(count - 1);
    }
}

void TextBrowser::hideEvent(QHideEvent* event)
{
    QTextBrowser::hideEvent(event);
    d.ub = -1;
}

void TextBrowser::keyPressEvent(QKeyEvent* event)
{
    // for example:
    // - Ctrl+C goes to the browser
    // - Ctrl+V goes to the buddy
    // - Shift+7 ("/") goes to the buddy
    switch (event->key()) {
        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Meta:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
            break;
        default:
            if (!event->matches(QKeySequence::Copy) && !event->matches(QKeySequence::SelectAll)) {
                QApplication::sendEvent(d.bud, event);
                d.bud->setFocus();
                return;
            }
            break;
    }
    QTextBrowser::keyPressEvent(event);
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
    const QRect bounds = rect().translated(horizontalScrollBar()->value(),
                                           verticalScrollBar()->value());

    if (!d.highlights.isEmpty()) {
        QPainter painter(viewport());
        painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());

        int margin = qCeil(document()->documentMargin());
        foreach (int highlight, d.highlights) {
            QTextBlock block = document()->findBlockByNumber(highlight);
            QRectF br = document()->documentLayout()->blockBoundingRect(block);
            if (bounds.intersects(br.toAlignedRect()))
                painter.fillRect(br.adjusted(-margin, 0, margin, 0), d.highlightColor);
        }
    }

    QTextBrowser::paintEvent(event);

    QPainter painter(viewport());
    painter.save();
    painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());

    int last = -1;
    if (!d.markers.isEmpty()) {
        painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
        foreach (int marker, d.markers) {
            QTextBlock block = document()->findBlockByNumber(marker);
            QRectF br = document()->documentLayout()->blockBoundingRect(block);
            if (bounds.intersects(br.toAlignedRect())) {
                painter.drawLine(br.topLeft(), br.topRight());
                last = qMax(marker, last);
            }
        }
    }

    if (d.ub > 0 && d.ub >= last) {
        painter.setPen(QPen(d.markerColor, 1, Qt::DashLine));
        QTextBlock block = document()->findBlockByNumber(d.ub);
        QRectF br = document()->documentLayout()->blockBoundingRect(block);
        if (bounds.intersects(br.toAlignedRect()))
            painter.drawLine(br.topLeft(), br.topRight());
    }

    QLinearGradient gradient(0, 0, 0, 3);
    gradient.setColorAt(0.0, d.shadowColor);
    gradient.setColorAt(1.0, Qt::transparent);
    painter.restore();
    painter.fillRect(0, 0, width(), 3, gradient);
}

void TextBrowser::wheelEvent(QWheelEvent* event)
{
#ifdef Q_OS_MAC
    // disable cmd+wheel zooming on mac
    QAbstractScrollArea::wheelEvent(event);
#else
    QTextBrowser::wheelEvent(event);
#endif // Q_OS_MAC
}

