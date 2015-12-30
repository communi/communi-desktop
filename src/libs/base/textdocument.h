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

    QDateTime timestamp() const;
    void setTimestamp(const QDateTime& timestamp);

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

    QString formatEvents(const QList<MessageData>& events) const;
    QString formatSummary(const QList<MessageData>& events) const;
    QString formatBlock(const QDateTime& timestamp, const QString& message) const;

    friend class TextBrowser;

    struct Private {
        int uc;
        int dirty;
        bool clone;
        bool batch;
        int rebuild;
        QString css;
        int lowlight;
        bool visible;
        IrcBuffer* buffer;
        QDateTime timestamp;
        QList<int> highlights;
        QString timeStampFormat;
        QList<MessageData> queue;
        MessageFormatter* formatter;
    } d;
};

Q_DECLARE_METATYPE(TextDocument*)

#endif // TEXTDOCUMENT_H
