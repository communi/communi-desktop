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

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <QTextDocument>
#include <QStringList>
#include <IrcMessage>
#include <QMetaType>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include "messageformatter.h"

class IrcBuffer;

struct MessageData : MessageFormat
{
    MessageData() : own(false), type(IrcMessage::Unknown) { }

    bool isEmpty() const { return plainText.isEmpty(); }
    bool isEvent() const { return type == IrcMessage::Join ||
                                  type == IrcMessage::Mode ||
                                  type == IrcMessage::Nick ||
                                  type == IrcMessage::Part ||
                                  type == IrcMessage::Quit ||
                                  type == IrcMessage::Topic; }

    bool canMerge(const MessageData& other) const {
        return isEvent() && (!own || type != IrcMessage::Join)
               && other.isEvent() && (!other.own || other.type != IrcMessage::Join);
    }

    static MessageData format(MessageFormatter* formatter, IrcMessage* message) {
        MessageData data;
        data.MessageFormat::operator=(formatter->formatMessage(message));
        data.timestamp = message->timeStamp();
        data.nick = message->nick();
        data.type = message->type();
        data.own = message->isOwn();
        if (data.isEvent())
            data.events += data;
        return data;
    }

    bool own;
    QString nick;
    QDateTime timestamp;
    IrcMessage::Type type;
    QList<MessageData> events;
};

class TextDocument : public QTextDocument
{
    Q_OBJECT

public:
    explicit TextDocument(IrcBuffer* buffer);

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    QString styleSheet() const;
    void setStyleSheet(const QString& css);

    TextDocument* clone();
    bool isClone() const;

    IrcBuffer* buffer() const;
    MessageFormatter* formatter() const;

    int totalCount() const;

    bool isVisible() const;
    void setVisible(bool visible);

    void drawBackground(QPainter* painter, const QRect& bounds);
    void drawForeground(QPainter* painter, const QRect& bounds);

public slots:
    void reset();
    void lowlight(int block = -1);
    void addHighlight(int block = -1);
    void removeHighlight(int block);
    void append(const MessageData& message);
    void receiveMessage(IrcMessage* message);

signals:
    void lineRemoved(int height);
    void messageReceived(IrcMessage* message);
    void messageHighlighted(IrcMessage* message);
    void privateMessageReceived(IrcMessage* message);

protected:
    void updateBlock(int number);
    void timerEvent(QTimerEvent* event);

private slots:
    void flush();
    void rebuild();

private:
    void scheduleRebuild();
    void shiftLights(int diff);
    void insert(QTextCursor& cursor, const MessageData& data);

    MessageData mergeEvents(const QList<MessageData>& events) const;
    QString formatBlock(const QDateTime& timestamp, const QString& message, const QString& href = QString()) const;

    struct Private {
        int uc;
        int dirty;
        bool clone;
        int rebuild;
        QString css;
        int lowlight;
        bool visible;
        IrcBuffer* buffer;
        QList<int> highlights;
        QString timeStampFormat;
        QList<MessageData> queue;
        QList<MessageData> allLines;
        MessageFormatter* formatter;
    } d;
};

Q_DECLARE_METATYPE(TextDocument*)

#endif // TEXTDOCUMENT_H
