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

#include "textdocument.h"
#include "textbrowser.h"
#include "messageformatter.h"
#include "syntaxhighlighter.h"
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QTextCursor>
#include <QTextBlock>
#include <IrcBuffer>
#include <QPalette>
#include <QPainter>
#include <qmath.h>

static int delay = 1000;

TextDocument::TextDocument(IrcBuffer* buffer) : QTextDocument(buffer)
{
    qRegisterMetaType<TextDocument*>();

    d.ub = -1;
    d.dirty = -1;
    d.lowlight = -1;
    d.buffer = buffer;

    // TODO: stylesheet
    d.markerColor = Qt::darkGray;
    d.lowlightColor = qApp->palette().color(QPalette::AlternateBase);
    d.highlightColor = QColor("#ffe6e6");

    d.highlighter = new SyntaxHighlighter(this);

    d.formatter = new MessageFormatter(this);
    d.formatter->setBuffer(buffer);

    setUndoRedoEnabled(false);
    setMaximumBlockCount(1000);

    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));
    connect(documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(scrollToBottom()));
}

IrcBuffer* TextDocument::buffer() const
{
    return d.buffer;
}

SyntaxHighlighter* TextDocument::highlighter() const
{
    return d.highlighter;
}

int TextDocument::totalCount() const
{
    int count = d.lines.count();
    if (!isEmpty())
        count += blockCount();
    return count;
}

QColor TextDocument::markerColor() const
{
    return d.markerColor;
}

void TextDocument::setMarkerColor(const QColor& color)
{
    if (d.markerColor != color) {
        d.markerColor = color;
        // TODO: update();
    }
}

QColor TextDocument::lowlightColor() const
{
    return d.lowlightColor;
}

void TextDocument::setLowlightColor(const QColor& color)
{
    if (d.lowlightColor != color) {
        d.lowlightColor = color;
        // TODO: update();
    }
}

QColor TextDocument::highlightColor() const
{
    return d.highlightColor;
}

void TextDocument::setHighlightColor(const QColor& color)
{
    if (d.highlightColor != color) {
        d.highlightColor = color;
        // TODO: update();
    }
}

void TextDocument::ref(TextBrowser* browser)
{
    if (d.dirty > 0 && d.browsers.isEmpty())
        flushLines();
    d.browsers.insert(browser);
}

void TextDocument::deref(TextBrowser* browser)
{
    d.browsers.remove(browser);
    if (d.browsers.isEmpty())
        d.ub = -1;
}

void TextDocument::beginLowlight()
{
    d.lowlight = totalCount();
    d.lowlights.insert(d.lowlight, -1);
}

void TextDocument::endLowlight()
{
    d.lowlights.insert(d.lowlight, totalCount());
    d.lowlight = -1;
}

void TextDocument::addHighlight(int block)
{
    const int max = totalCount() - 1;
    if (block == -1)
        block = max;
    if (block >= 0 && block <= max) {
        QList<int>::iterator it = qLowerBound(d.highlights.begin(), d.highlights.end(), block);
        d.highlights.insert(it, block);
        updateBlock(block);
    }
}

void TextDocument::removeHighlight(int block)
{
    if (d.highlights.removeOne(block) && block >= 0 && block < totalCount())
        updateBlock(block);
}

void TextDocument::append(const QString& text)
{
    if (!text.isEmpty()) {
        if (d.dirty == 0 || !d.browsers.isEmpty()) {
            QTextCursor cursor(this);
            cursor.beginEditBlock();
            appendLine(cursor, text);
            cursor.endEditBlock();
        } else {
            if (d.dirty <= 0) {
                d.dirty = startTimer(delay);
                delay += 1000;
            }
            d.lines += text;
        }
    }
}

void TextDocument::drawForeground(QPainter* painter, const QRect& bounds)
{
    if (d.ub <= 1)
        return;

    const QPen oldPen = painter->pen();
    const QBrush oldBrush = painter->brush();

    painter->setPen(QPen(d.markerColor, 1, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);

    QTextBlock block = findBlockByNumber(d.ub);
    if (block.isValid()) {
        QRect br = documentLayout()->blockBoundingRect(block).toAlignedRect();
        if (bounds.intersects(br))
            painter->drawLine(br.topLeft(), br.topRight());
    }

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

void TextDocument::drawBackground(QPainter* painter, const QRect& bounds)
{
    if (d.lowlights.isEmpty() && d.highlights.isEmpty())
        return;

    const QPen oldPen = painter->pen();
    const QBrush oldBrush = painter->brush();
    const qreal oldOpacity = painter->opacity();
    const int margin = qCeil(documentMargin());
    const QAbstractTextDocumentLayout* layout = documentLayout();

    painter->setOpacity(0.35);
    painter->setBrush(d.lowlightColor);
    QMap<int, int>::const_iterator it;
    for (it = d.lowlights.begin(); it != d.lowlights.end(); ++it) {
        QTextBlock from = findBlockByNumber(it.key());
        QTextBlock to = findBlockByNumber(it.value());
        if (from.isValid() && to.isValid()) {
            QRect br = layout->blockBoundingRect(from).toAlignedRect();
            br = br.united(layout->blockBoundingRect(to).toAlignedRect());
            if (bounds.intersects(br)) {
                const bool last = to == lastBlock();
                if (last)
                    br.setBottom(bounds.bottom());
                br = br.adjusted(-margin, 0, margin, 0);
                painter->setPen(Qt::NoPen);
                painter->drawRect(br);
                painter->setPen(d.markerColor);
                painter->drawLine(br.topLeft(), br.topRight());
                if (!last)
                    painter->drawLine(br.bottomLeft(), br.bottomRight());
            }
        }
    }

    painter->setOpacity(0.75);
    painter->setPen(Qt::NoPen);
    painter->setBrush(d.highlightColor);
    foreach (int highlight, d.highlights) {
        QTextBlock block = findBlockByNumber(highlight);
        if (block.isValid()) {
            QRect br = layout->blockBoundingRect(block).toAlignedRect();
            if (bounds.intersects(br))
                painter->drawRect(br.adjusted(-margin, 0, margin, 0));
        }
    }

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
    painter->setOpacity(oldOpacity);
}

void TextDocument::updateBlock(int number)
{
    if (!d.browsers.isEmpty()) {
        QTextBlock block = findBlockByNumber(number);
        if (block.isValid())
            QMetaObject::invokeMethod(documentLayout(), "updateBlock", Q_ARG(QTextBlock, block));
    }
}

void TextDocument::timerEvent(QTimerEvent* event)
{
    QTextDocument::timerEvent(event);
    if (event->timerId() == d.dirty) {
        delay -= 1000;
        flushLines();
    }
}

void TextDocument::flushLines()
{
    if (!d.lines.isEmpty()) {
        QTextCursor cursor(this);
        cursor.beginEditBlock();
        foreach (const QString& line, d.lines)
            appendLine(cursor, line);
        cursor.endEditBlock();
        d.lines.clear();
    }

    if (d.dirty > 0) {
        killTimer(d.dirty);
        d.dirty = 0;
    }
}

void TextDocument::scrollToBottom()
{
    foreach (TextBrowser* browser, d.browsers) {
        if (browser->isAtBottom())
            QMetaObject::invokeMethod(browser, "scrollToBottom", Qt::QueuedConnection);
    }
}

void TextDocument::receiveMessage(IrcMessage* message)
{
    append(d.formatter->formatMessage(message));
    emit messageReceived(message);
}

void TextDocument::appendLine(QTextCursor& cursor, const QString& line)
{
    const int count = blockCount();
    cursor.movePosition(QTextCursor::End);
    if (!isEmpty()) {
        const int max = maximumBlockCount();
        cursor.insertBlock();
        if (count >= max) {
            const int diff = max - count + 1;
            if (d.ub > 0)
                d.ub -= diff;
            QList<int>::iterator i;
            for (i = d.highlights.begin(); i != d.highlights.end(); ++i) {
                *i -= diff;
                if (*i < 0)
                    i = d.highlights.erase(i);
            }
            QMap<int, int> ll;
            QMap<int, int>::const_iterator j;
            for (j = d.lowlights.begin(); j != d.lowlights.end(); ++j) {
                int from = j.key() - diff;
                int to = j.value() - diff;
                if (to > 0)
                    ll.insert(qMax(0, from), to);
            }
            d.lowlights = ll;
        }
    }

#if QT_VERSION >= 0x040800
    QTextBlockFormat format = cursor.blockFormat();
    format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
    cursor.setBlockFormat(format);
#endif // QT_VERSION

    cursor.insertHtml(line);

    if (d.ub == -1 && d.browsers.isEmpty())
        d.ub = count;

    const int bn = cursor.block().blockNumber();
    QMap<int, int>::const_iterator it;
    for (it = d.lowlights.begin(); it != d.lowlights.end(); ++it) {
        if (bn >= it.key() && (it.value() == -1 || bn <= it.value())) {
            cursor.block().setUserState(1);
            break;
        }
    }
}
