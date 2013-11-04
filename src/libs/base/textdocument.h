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

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <QTextDocument>
#include <QStringList>
#include <QMetaType>
#include <QSet>

class IrcBuffer;
class IrcMessage;
class TextBrowser;
class MessageFormatter;

class TextDocument : public QTextDocument
{
    Q_OBJECT

public:
    explicit TextDocument(IrcBuffer* buffer);

    IrcBuffer* buffer() const;

    int totalCount() const;

    void ref(TextBrowser* browser);
    void deref(TextBrowser* browser);

    int note() const;
    void setNote(int note);

    void addMarker(int block = -1);
    void removeMarker(int block);

    void addHighlight(int block = -1);
    void removeHighlight(int block);

    void drawMarkers(QPainter* painter, const QRect& bounds);
    void drawHighlights(QPainter* painter, const QRect& bounds);

public slots:
    void append(const QString& text);

protected:
    void updateBlock(int number);
    void timerEvent(QTimerEvent* event);

private slots:
    void flushLines();
    void scrollToBottom();
    void receiveMessage(IrcMessage* message);

private:
    void appendLine(QTextCursor& cursor, const QString& line);

    struct Private {
        int note;
        int dirty;
        IrcBuffer* buffer;
        QStringList lines;
        QList<int> markers;
        QList<int> highlights;
        MessageFormatter* formatter;
        QSet<TextBrowser*> browsers;
    } d;
};

Q_DECLARE_METATYPE(TextDocument*)

#endif // TEXTDOCUMENT_H
