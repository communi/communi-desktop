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
#include <QMetaType>
#include <QDateTime>
#include "messagedata.h"

class IrcBuffer;
class IrcMessage;
class MessageData;
class MessageFormatter;

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

    QString tooltip(const QPoint& pos) const;

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

    MessageData formatMessage(IrcMessage* message) const;
    QString formatEvents(const QList<MessageData>& events) const;
    QString formatSummary(const QList<MessageData>& events) const;
    QString formatBlock(const QDateTime& timestamp, const QString& message) const;

    friend class TextBrowser;

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
        MessageFormatter* formatter;
    } d;
};

Q_DECLARE_METATYPE(TextDocument*)

#endif // TEXTDOCUMENT_H
