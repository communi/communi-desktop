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

#include "textdocument.h"
#include "eventformatter.h"
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>
#include <QTextBlockUserData>
#include <IrcConnection>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOption>
#include <QTextCursor>
#include <QTextBlock>
#include <IrcMessage>
#include <IrcBuffer>
#include <QPalette>
#include <QPointer>
#include <QPainter>
#include <QFrame>
#include <qmath.h>

static int delay = 1000;

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

struct TextBlockMessageData : QTextBlockUserData
{
    TextBlockMessageData(const MessageData& data) : data(data) { }
    MessageData data;
};

TextDocument::TextDocument(IrcBuffer* buffer) : QTextDocument(buffer)
{
    qRegisterMetaType<TextDocument*>();

    d.uc = 0;
    d.dirty = -1;
    d.rebuild = -1;
    d.lowlight = -1;
    d.clone = false;
    d.batch = false;
    d.buffer = buffer;
    d.visible = false;

    d.formatter = new MessageFormatter(this);
    connect(d.formatter, SIGNAL(formatted(MessageData)), this, SLOT(append(MessageData)));
    d.formatter->setBuffer(buffer);

    setUndoRedoEnabled(false);
    setMaximumBlockCount(1000);

    connect(buffer->connection(), SIGNAL(disconnected()), this, SLOT(lowlight()));
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
        scheduleRebuild();
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
        scheduleRebuild();
    }
}

TextDocument* TextDocument::clone()
{
    if (d.dirty > 0)
        flush();

    TextDocument* doc = new TextDocument(d.buffer);
    doc->setDefaultStyleSheet(defaultStyleSheet());
    QTextCursor(doc).insertFragment(QTextDocumentFragment(this));
    doc->rootFrame()->setFrameFormat(rootFrame()->frameFormat());

    // TODO:
    doc->d.uc = d.uc;
    doc->d.css = d.css;
    doc->d.lowlight = d.lowlight;
    doc->d.buffer = d.buffer;
    doc->d.highlights = d.highlights;
    doc->d.timeStampFormat = d.timeStampFormat;
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
    int count = d.queue.count();
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
                flush();
        } else {
            d.uc = 0;
            if (TextBlockMessageData* block = static_cast<TextBlockMessageData*>(lastBlock().userData()))
                d.timestamp = block->data.timestamp();
        }
        d.visible = visible;
    }
}

QDateTime TextDocument::timestamp() const
{
    if (d.visible) {
        if (TextBlockMessageData* block = static_cast<TextBlockMessageData*>(lastBlock().userData()))
            return block->data.timestamp();
    }
    return d.timestamp;
}

void TextDocument::setTimestamp(const QDateTime& timestamp)
{
    d.timestamp = timestamp;
}

void TextDocument::lowlight(int block)
{
    if (block == -1)
        block = totalCount() - 1;
    if (d.lowlight != block) {
        d.lowlight = block;
        updateBlock(block);
    }
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
    d.uc = 0;
    d.lowlight = -1;
    d.highlights.clear();
    d.queue.clear();
}

void TextDocument::append(const MessageData& data)
{
    if (!data.isEmpty()) {
        MessageData last;
        if (!d.queue.isEmpty())
            last = d.queue.last();
        else if (TextBlockMessageData* block = static_cast<TextBlockMessageData*>(lastBlock().userData()))
            last = block->data;

        if (!last.isEmpty() && data.type() != IrcMessage::Unknown && data.timestamp().date() != last.timestamp().date()) {
            MessageData dc;
            dc.setFormat(QString("<p class='date'>%1</p>").arg(data.timestamp().date().toString(Qt::ISODate)));
            append(dc);
        }

        MessageData msg = data;
        const bool merge = last.canMerge(data);
        if (merge) {
            msg.merge(last);
            msg.setFormat(formatSummary(msg.getEvents()));
            if (!d.queue.isEmpty())
                d.queue.replace(d.queue.count() - 1, msg);
        } else {
            if (d.timestamp < data.timestamp())
                ++d.uc;
            else
                d.uc = 0;
        }
        if (!d.batch && (d.dirty == 0 || d.visible)) {
            QTextCursor cursor(this);
            cursor.beginEditBlock();
            if (merge) {
                cursor.movePosition(QTextCursor::End);
                cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                cursor.deletePreviousChar();
            }
            insert(cursor, msg);
            cursor.endEditBlock();
        } else {
            if (!d.batch && d.dirty <= 0) {
                d.dirty = startTimer(delay);
                delay += 1000;
            }
            if (!merge)
                d.queue += msg;
        }
    }
}

void TextDocument::drawForeground(QPainter* painter, const QRect& bounds)
{
    const int num = blockCount() - d.uc;
    if (num > 0) {
        const QPen oldPen = painter->pen();
        const QBrush oldBrush = painter->brush();
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QPalette().color(QPalette::Mid), 1, Qt::DashLine));
        QTextBlock block = findBlockByNumber(num);
        if (block.isValid()) {
            QRect br = documentLayout()->blockBoundingRect(block).toAlignedRect();
            if (bounds.intersects(br)) {
                QLine line(br.topLeft(), br.topRight());
                line.translate(0, -2);
                painter->drawLine(line);
            }
        }
        painter->setPen(oldPen);
        painter->setBrush(oldBrush);
    }
}

void TextDocument::drawBackground(QPainter* painter, const QRect& bounds)
{
    if (d.highlights.isEmpty() && d.lowlight == -1)
        return;

    const int margin = qCeil(documentMargin());
    const QAbstractTextDocumentLayout* layout = documentLayout();

    static QPointer<TextLowlight> lowlightFrame = 0;
    if (!lowlightFrame)
        lowlightFrame = new TextLowlight(static_cast<QWidget*>(painter->device()));

    static QPointer<TextHighlight> highlightFrame = 0;
    if (!highlightFrame)
        highlightFrame = new TextHighlight(static_cast<QWidget*>(painter->device()));

    if (d.lowlight != -1) {
        const QAbstractTextDocumentLayout* layout = documentLayout();
        const int margin = qCeil(documentMargin());
        const QTextBlock to = findBlockByNumber(d.lowlight);
        if (to.isValid()) {
            QRect br = layout->blockBoundingRect(to).toAlignedRect();
            br.setTop(0);
            if (bounds.intersects(br)) {
                br.adjust(-margin - 1, 0, margin + 1, 2);
                painter->translate(br.topLeft());
                lowlightFrame->setGeometry(br);
                lowlightFrame->render(painter);
                painter->translate(-br.topLeft());
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

QString TextDocument::tooltip(const QPoint& point) const
{
    const int pos = documentLayout()->hitTest(point, Qt::FuzzyHit);
    const QTextBlock block = findBlock(pos);
    TextBlockMessageData* blockData = static_cast<TextBlockMessageData*>(block.userData());
    if (blockData)
        return formatEvents(blockData->data.getEvents());
    return QString();
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
        flush();
    } else if (event->timerId() == d.rebuild) {
        rebuild();
    }
}

void TextDocument::flush()
{
    if (!d.queue.isEmpty()) {
        QTextCursor cursor(this);
        cursor.beginEditBlock();
        foreach (const MessageData& data, d.queue)
            insert(cursor, data);
        cursor.endEditBlock();
        d.queue.clear();
    }

    if (d.dirty > 0) {
        killTimer(d.dirty);
        d.dirty = 0;
    }
}

void TextDocument::receiveMessage(IrcMessage* message)
{
    if (message->type() == IrcMessage::Batch) {
        IrcBatchMessage* batch = static_cast<IrcBatchMessage*>(message);
        d.batch = true;
        foreach (IrcMessage* msg, batch->messages())
            receiveMessage(msg);
        d.batch = false;
        if (!d.queue.isEmpty()) {
            if (d.visible) {
                flush();
            } else if (d.dirty <= 0) {
                d.dirty = startTimer(delay);
                delay += 1000;
            }
        }
    } else {
        MessageData data = d.formatter->formatMessage(message);
        if (!data.isEmpty()) {
            append(data);

            if (data.type() == IrcMessage::Private || data.type() == IrcMessage::Notice) {
                bool unseen = d.timestamp < message->timeStamp();
                if (unseen)
                    emit messageReceived(message);

                if (!message->isOwn()) {
                    QString content;
                    bool priv = false;
                    if (data.type() == IrcMessage::Private) {
                        IrcPrivateMessage* pm = static_cast<IrcPrivateMessage*>(message);
                        content = pm->content();
                        priv = pm->isPrivate();
                    } else {
                        IrcNoticeMessage* nm = static_cast<IrcNoticeMessage*>(message);
                        content = nm->content();
                        priv = nm->isPrivate();
                    }
                    IrcConnection* connection = message->connection();
                    const bool contains = content.contains(connection->nickName(), Qt::CaseInsensitive);
                    if (contains) {
                        if (connection->isConnected())
                            addHighlight(totalCount() - 1);
                        if (unseen)
                            emit messageHighlighted(message);
                    } else if (unseen && priv && connection->isConnected()) {
                        emit privateMessageReceived(message);
                    }
                }
            }
        }
    }
}

void TextDocument::rebuild()
{
    QList<MessageData> lines;
    QTextBlock block = firstBlock();
    while (block.isValid()) {
        TextBlockMessageData* blockData = static_cast<TextBlockMessageData*>(block.userData());
        if (blockData)
            lines += blockData->data;
        block = block.next();
    }
    clear();
    d.queue = lines;
    flush();
    if (d.rebuild > 0) {
        killTimer(d.rebuild);
        d.rebuild = 0;
    }
}

void TextDocument::scheduleRebuild()
{
    if (d.rebuild < 0 && !isEmpty())
        d.rebuild = startTimer(isVisible() ? 0 : 1000);
}

void TextDocument::shiftLights(int diff)
{
    QList<int>::iterator it = d.highlights.begin();
    while (it != d.highlights.end()) {
        *it -= diff;
        if (*it < 0)
            it = d.highlights.erase(it);
        else
            ++it;
    }
    d.lowlight -= diff;
}

void TextDocument::insert(QTextCursor& cursor, const MessageData& data)
{
    cursor.movePosition(QTextCursor::End);

    if (!isEmpty()) {
        const int count = blockCount();
        const int max = maximumBlockCount();
        const QRectF br = documentLayout()->blockBoundingRect(findBlockByNumber(0));
        cursor.insertBlock();

        if (count >= max) {
            emit lineRemoved(qRound(br.bottom()));
            shiftLights(max - count + 1);
        }
    }

    cursor.insertHtml(formatBlock(data.timestamp(), data.format()));
    cursor.block().setUserData(new TextBlockMessageData(data));

    QTextBlockFormat format = cursor.blockFormat();
    format.setLineHeight(125, QTextBlockFormat::ProportionalHeight);
    if (data.type() == IrcMessage::Unknown)
        format.setAlignment(Qt::AlignRight);
    else
        format.setAlignment(Qt::AlignLeft);
    cursor.setBlockFormat(format);
}

QString TextDocument::formatEvents(const QList<MessageData>& events) const
{
    EventFormatter formatter;
    formatter.setBuffer(d.buffer);

    QStringList lines;
    foreach (const MessageData& event, events) {
        if (!event.isEmpty()) {
            IrcMessage* msg = IrcMessage::fromData(event.data(), d.buffer->connection());
            lines += formatBlock(event.timestamp(), formatter.formatMessage(msg).format());
            delete msg;
        }
    }
    if (!lines.isEmpty())
        return tr("<html><head><style>%1</style></head><body style='white-space:pre'>%2</body></html>").arg(d.css, lines.join(tr("<br/>")));
    return QString();
}

QString TextDocument::formatSummary(const QList<MessageData>& events) const
{
    QStringList actions;
    QStringList changes;
    QSet<QString> nicks;
    QSet<IrcMessage::Type> handled;
    EventFormatter formatter;

    foreach (const MessageData& event, events) {
        switch (event.type()) {
        case IrcMessage::Join:
            if (!handled.contains(event.type()))
                actions += tr("joined");
            break;
        case IrcMessage::Part:
            if (!handled.contains(event.type()))
                actions += tr("left");
            break;
        case IrcMessage::Quit:
            if (event.isError()) {
                if (!handled.contains(IrcMessage::Error))
                    actions += tr("disconnected");
                nicks.insert(event.nick());
                handled.insert(IrcMessage::Error);
                continue;
            } else {
                if (!handled.contains(event.type()))
                    actions += tr("quit");
            }
            break;
        case IrcMessage::Kick:
            if (!handled.contains(event.type()))
                actions += tr("kicked");
            break;
        case IrcMessage::Nick:
            if (!handled.contains(event.type()))
                changes += tr("nick");
            break;
        case IrcMessage::Mode:
            if (!handled.contains(event.type()))
                changes += tr("mode");
            break;
        case IrcMessage::Topic:
            if (!handled.contains(event.type()))
                changes += tr("topic");
            break;
        default:
            break;
        }
        nicks.insert(event.nick());
        handled.insert(event.type());
    }

    if (!changes.isEmpty())
        actions += tr("changed %1").arg(changes.join(tr(" and ")));

    if (actions.count() > 2)
        actions = QStringList() << QStringList(actions.mid(0, actions.count() - 1)).join(tr(", ")) << actions.last();

    if (nicks.count() == 1)
        return formatter.formatEvent(tr("%1 %2").arg(formatter.styledText(*nicks.begin(), MessageFormatter::Bold),
                                                     actions.join(tr(" and "))));

    return formatter.formatEvent(tr("%1 %2").arg(formatter.styledText(tr("%1 users").arg(nicks.count()), MessageFormatter::Bold),
                                                 actions.join(tr(" or "))));
}

QString TextDocument::formatBlock(const QDateTime& timestamp, const QString& message) const
{
    if (message.isEmpty())
        return QString();

    const QString time = timestamp.time().toString(d.timeStampFormat);
    return tr("<span class='timestamp'>%1</span> %2").arg(time, message);
}

#include "textdocument.moc"
