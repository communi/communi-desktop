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
#include <QAbstractTextDocumentLayout>
#include <QTextCursor>
#include <QTextBlock>
#include <IrcBuffer>
#include <QPainter>
#include <qmath.h>

static int delay = 1000;

TextDocument::TextDocument(IrcBuffer* buffer) : QTextDocument(buffer)
{
    qRegisterMetaType<TextDocument*>();

    d.ub = -1;
    d.dirty = -1;
    d.buffer = buffer;

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

int TextDocument::totalCount() const
{
    int count = d.lines.count();
    if (!isEmpty())
        count += blockCount();
    return count;
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

void TextDocument::addMarker(int block)
{
    const int max = totalCount() - 1;
    if (block == -1)
        block = max;
    if (block >= 0 && block <= max) {
        QList<int>::iterator it = qLowerBound(d.markers.begin(), d.markers.end(), block);
        d.markers.insert(it, block);
        updateBlock(block);
    }
}

void TextDocument::removeMarker(int block)
{
    if (d.markers.removeOne(block) && block >= 0 && block < totalCount())
        updateBlock(block);
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

void TextDocument::drawMarkers(QPainter* painter, const QRect& bounds)
{
    int last = -1;
    if (!d.markers.isEmpty()) {
        QAbstractTextDocumentLayout* layout = documentLayout();
        foreach (int marker, d.markers) {
            QTextBlock block = findBlockByNumber(marker);
            if (block.isValid() && block != lastBlock()) {
                QRect br = layout->blockBoundingRect(block).toAlignedRect();
                if (bounds.intersects(br)) {
                    painter->drawLine(br.topLeft(), br.topRight());
                    last = qMax(marker, last);
                }
            }
        }
    }

    if (d.ub > 0 && d.ub >= last) {
        QTextBlock block = findBlockByNumber(d.ub);
        if (block.isValid()) {
            QRect br = documentLayout()->blockBoundingRect(block).toAlignedRect();
            if (bounds.intersects(br))
                painter->drawLine(br.topLeft(), br.topRight());
        }
    }
}

void TextDocument::drawHighlights(QPainter* painter, const QRect& bounds)
{
    if (!d.highlights.isEmpty()) {
        int margin = qCeil(documentMargin());
        QAbstractTextDocumentLayout* layout = documentLayout();
        foreach (int highlight, d.highlights) {
            QTextBlock block = findBlockByNumber(highlight);
            if (block.isValid()) {
                QRect br = layout->blockBoundingRect(block).toAlignedRect();
                if (bounds.intersects(br))
                    painter->drawRect(br.adjusted(-margin, 0, margin, 0));
            }
        }
    }
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

static void decrementHelper(QList<int>& lst, int d)
{
    QList<int>::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        *it -= d;
        if (*it < 0)
            it = lst.erase(it);
    }
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
            decrementHelper(d.markers, diff);
            decrementHelper(d.highlights, diff);
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
}
