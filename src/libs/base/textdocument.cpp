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

#include "textdocument.h"
#include "messageformatter.h"
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>
#include <QTextBlockUserData>
#include <IrcConnection>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOption>
#include <QTextCursor>
#include <QTextBlock>
#include <IrcBuffer>
#include <QPalette>
#include <QPointer>
#include <QPainter>
#include <QFrame>
#include <qmath.h>

static int delay = 1000;

class TextBlockData : public QTextBlockUserData
{
public:
    QString message;
    QDateTime timestamp;
};

class TextFrame : public QFrame
{
public:
    TextFrame(QWidget* parent = 0) : QFrame(parent)
    {
        setVisible(false);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
    }

    void paintEvent(QPaintEvent*)
    {
        QStyleOption option;
        option.init(this);
        QStylePainter painter(this);
        painter.drawPrimitive(QStyle::PE_Widget, option);
    }
};

class TextHighlight : public TextFrame
{
    Q_OBJECT
public:
    TextHighlight(QWidget* parent = 0) : TextFrame(parent) { }
};

class TextLowlight : public TextFrame
{
    Q_OBJECT
public:
    TextLowlight(QWidget* parent = 0) : TextFrame(parent) { }
};

TextDocument::TextDocument(IrcBuffer* buffer) : QTextDocument(buffer)
{
    qRegisterMetaType<TextDocument*>();

    d.ub = -1;
    d.dirty = -1;
    d.lowlight = -1;
    d.clone = false;
    d.drawUb = false;
    d.buffer = buffer;
    d.visible = false;

    d.formatter = new MessageFormatter(this);
    d.formatter->setTimeStampFormat(QString());
    d.formatter->setBuffer(buffer);

    setUndoRedoEnabled(false);
    setMaximumBlockCount(1000);

    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));
}

QString TextDocument::timeStampFormat() const
{
    return d.timeStampFormat;
}

void TextDocument::setTimeStampFormat(const QString& format)
{
    if (d.timeStampFormat != format) {
        d.timeStampFormat = format;
        rebuild();
    }
}

QString TextDocument::styleSheet() const
{
    return d.css;
}

void TextDocument::setStyleSheet(const QString& css)
{
    if (d.css != css) {
        d.css = css;
        setDefaultStyleSheet(css);
        rebuild();
    }
}

TextDocument* TextDocument::clone()
{
    if (d.dirty > 0)
        flushLines();

    TextDocument* doc = new TextDocument(d.buffer);
    doc->setDefaultStyleSheet(defaultStyleSheet());
    QTextCursor(doc).insertFragment(QTextDocumentFragment(this));
    doc->rootFrame()->setFrameFormat(rootFrame()->frameFormat());

    // TODO:
    doc->d.ub = d.ub;
    doc->d.css = d.css;
    doc->d.lowlight = d.lowlight;
    doc->d.buffer = d.buffer;
    doc->d.highlights = d.highlights;
    doc->d.lowlights = d.lowlights;
    doc->d.clone = true;

    return doc;
}

bool TextDocument::isClone() const
{
    return d.clone;
}

IrcBuffer* TextDocument::buffer() const
{
    return d.buffer;
}

MessageFormatter* TextDocument::formatter() const
{
    return d.formatter;
}

int TextDocument::totalCount() const
{
    int count = d.lines.count();
    if (!isEmpty())
        count += blockCount();
    return count;
}

bool TextDocument::isVisible() const
{
    return d.visible;
}

void TextDocument::setVisible(bool visible)
{
    if (d.visible != visible) {
        if (visible) {
            if (d.dirty > 0)
                flushLines();
        } else {
            d.ub = -1;
        }
        d.visible = visible;
    }
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

void TextDocument::reset()
{
    d.ub = -1;
    d.lowlight = -1;
    d.lowlights.clear();
    d.highlights.clear();
}

void TextDocument::append(const QString& message, const QDateTime& timestamp)
{
    if (!message.isEmpty()) {
        TextBlockData* data = new TextBlockData;
        data->timestamp = timestamp;
        data->message = message;
        if (d.dirty == 0 || d.visible) {
            QTextCursor cursor(this);
            cursor.beginEditBlock();
            appendLine(cursor, data);
            cursor.endEditBlock();
        } else {
            if (d.dirty <= 0) {
                d.dirty = startTimer(delay);
                delay += 1000;
            }
            d.lines += data;
        }
    }
}

void TextDocument::drawForeground(QPainter* painter, const QRect& bounds)
{
    if (d.drawUb) {
        const QPen oldPen = painter->pen();
        const QBrush oldBrush = painter->brush();
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QPalette().color(QPalette::Mid), 1, Qt::DashLine));
        QTextBlock block = findBlockByNumber(d.ub);
        if (block.isValid()) {
            QRect br = documentLayout()->blockBoundingRect(block).toAlignedRect();
            if (bounds.intersects(br))
                painter->drawLine(br.topLeft(), br.topRight());
        }
        painter->setPen(oldPen);
        painter->setBrush(oldBrush);
    }
}

void TextDocument::drawBackground(QPainter* painter, const QRect& bounds)
{
    if (d.highlights.isEmpty() && d.lowlights.isEmpty())
        return;

    const int margin = qCeil(documentMargin());
    const QAbstractTextDocumentLayout* layout = documentLayout();

    static QPointer<TextLowlight> lowlightFrame = 0;
    if (!lowlightFrame)
        lowlightFrame = new TextLowlight(static_cast<QWidget*>(painter->device()));

    static QPointer<TextHighlight> highlightFrame = 0;
    if (!highlightFrame)
        highlightFrame = new TextHighlight(static_cast<QWidget*>(painter->device()));

    d.drawUb = d.ub > 1;
    if (!d.lowlights.isEmpty()) {
        const QAbstractTextDocumentLayout* layout = documentLayout();
        const int margin = qCeil(documentMargin());
        QMap<int, int>::const_iterator it;
        for (it = d.lowlights.begin(); it != d.lowlights.end(); ++it) {
            const QTextBlock from = findBlockByNumber(it.key());
            const QTextBlock to = findBlockByNumber(it.value());
            if (from.isValid() && to.isValid()) {
                const QRect fr = layout->blockBoundingRect(from).toAlignedRect();
                const QRect tr = layout->blockBoundingRect(to).toAlignedRect();
                QRect br = fr.united(tr);
                if (bounds.intersects(br)) {
                    bool atBottom = false;
                    if (to == lastBlock()) {
                        if (qAbs(bounds.bottom() - br.bottom()) < qMin(fr.height(), tr.height()))
                            atBottom = true;
                    }
                    if (atBottom)
                        br.setBottom(bounds.bottom() + 1);
                    br.adjust(-margin - 1, 0, margin + 1, 0);
                    painter->translate(br.topLeft());
                    lowlightFrame->setGeometry(br);
                    lowlightFrame->render(painter);
                    painter->translate(-br.topLeft());
                    if (d.drawUb && d.ub - 1 >= it.key() && (it.value() == -1 || d.ub - 1 <= it.value()))
                        d.drawUb = false;
                }
            }
        }
    }

    foreach (int highlight, d.highlights) {
        QTextBlock block = findBlockByNumber(highlight);
        if (block.isValid()) {
            QRect br = layout->blockBoundingRect(block).toAlignedRect();
            if (bounds.intersects(br)) {
                br.adjust(-margin - 1, 0, margin + 1, 2);
                painter->translate(br.topLeft());
                highlightFrame->setGeometry(br);
                highlightFrame->render(painter);
                painter->translate(-br.topLeft());
            }
        }
    }
}

void TextDocument::updateBlock(int number)
{
    if (d.visible) {
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
        foreach (TextBlockData* line, d.lines)
            appendLine(cursor, line);
        cursor.endEditBlock();
        d.lines.clear();
    }

    if (d.dirty > 0) {
        killTimer(d.dirty);
        d.dirty = 0;
    }
}

void TextDocument::receiveMessage(IrcMessage* message)
{
    append(d.formatter->formatMessage(message), message->timeStamp());
    emit messageReceived(message);

    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        if (!message->isOwn()) {
            const bool contains = message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive);
            if (contains) {
                addHighlight(totalCount() - 1);
                emit messageHighlighted(message);
            } else if (!d.visible && message->property("private").toBool()) {
                emit messageMissed(message);
            }
        }
    }
}

void TextDocument::rebuild()
{
    if (d.visible)
        flushLines();
    QTextBlock block = begin();
    while (block.isValid()) {
        TextBlockData* data = static_cast<TextBlockData*>(block.userData());
        if (data)
            d.lines += new TextBlockData(*data);
        block = block.next();
    }
    clear();
    if (d.visible)
        flushLines();
}

void TextDocument::appendLine(QTextCursor& cursor, TextBlockData* line)
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

    QString message = tr("<span class='timestamp'>%1</span> %2").arg(line->timestamp.time().toString(d.timeStampFormat)).arg(line->message);
    cursor.insertHtml(message);

    QTextBlock block = cursor.block();
    block.setUserData(line);

    QTextBlockFormat format = cursor.blockFormat();
    format.setLineHeight(130, QTextBlockFormat::ProportionalHeight);
    cursor.setBlockFormat(format);

    if (d.ub == -1 && !d.visible)
        d.ub = count;
}

#include "textdocument.moc"
