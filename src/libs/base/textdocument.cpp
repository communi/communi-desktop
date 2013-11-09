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

    d.note = -1;
    d.dirty = -1;
    d.buffer = buffer;

    d.formatter = new MessageFormatter(this);
    d.formatter->setBuffer(buffer);

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
    return blockCount() + d.lines.count();
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
}

int TextDocument::note() const
{
    return d.note;
}

void TextDocument::setNote(int note)
{
    if (d.note != note) {
        removeMarker(d.note);
        if (note != -1)
            addMarker(note);
        d.note = note;
    }
}

void TextDocument::addMarker(int block)
{
    if (block == -1)
        block = totalCount() - 1;
    d.markers.append(block);
    updateBlock(block);
}

void TextDocument::removeMarker(int block)
{
    if (d.markers.removeOne(block))
        updateBlock(block);
}

void TextDocument::addHighlight(int block)
{
    if (block == -1)
        block = totalCount() - 1;
    d.highlights.append(block);
    updateBlock(block);
}

void TextDocument::removeHighlight(int block)
{
    if (d.highlights.removeOne(block))
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
    if (!d.markers.isEmpty()) {
        QAbstractTextDocumentLayout* layout = documentLayout();
        foreach (int marker, d.markers) {
            QTextBlock block = findBlockByNumber(marker);
            if (block.isValid()) {
                QRect br = layout->blockBoundingRect(block).toAlignedRect();
                if (bounds.intersects(br))
                    painter->drawLine(br.topLeft(), br.topRight());
            }
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

void TextDocument::appendLine(QTextCursor& cursor, const QString& line)
{
    cursor.movePosition(QTextCursor::End);
    if (!isEmpty())
        cursor.insertBlock();

#if QT_VERSION >= 0x040800
    QTextBlockFormat format = cursor.blockFormat();
    format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
    cursor.setBlockFormat(format);
#endif // QT_VERSION

    cursor.insertHtml(line);
}
