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
#include <QMetaType>
#include <QDateTime>
#include <QMap>

class IrcBuffer;
class IrcMessage;
class TextBlockData;
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

    void beginLowlight();
    void endLowlight();

    void addHighlight(int block = -1);
    void removeHighlight(int block);

    void drawBackground(QPainter* painter, const QRect& bounds);
    void drawForeground(QPainter* painter, const QRect& bounds);

public slots:
    void reset();
    void append(const QString& message, const QDateTime& timestamp = QDateTime());
    void receiveMessage(IrcMessage* message);

signals:
    void messageMissed(IrcMessage* message);
    void messageReceived(IrcMessage* message);
    void messageHighlighted(IrcMessage* message);

protected:
    void updateBlock(int number);
    void timerEvent(QTimerEvent* event);

private slots:
    void flushLines();

private:
    void rebuild();
    void appendLine(QTextCursor& cursor, TextBlockData* line);

    struct Private {
        int ub;
        int dirty;
        bool clone;
        bool drawUb;
        QString css;
        int lowlight;
        bool visible;
        IrcBuffer* buffer;
        QList<int> highlights;
        QString timeStampFormat;
        QMap<int, int> lowlights;
        QList<TextBlockData*> lines;
        MessageFormatter* formatter;
    } d;
};

Q_DECLARE_METATYPE(TextDocument*)

#endif // TEXTDOCUMENT_H
