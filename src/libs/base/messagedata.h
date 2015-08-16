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

#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <QList>
#include <QString>
#include <QDateTime>
#include <IrcMessage>

class MessageData
{
public:
    MessageData();

    static IrcMessage::Type effectiveType(const IrcMessage* msg);

    bool isEmpty() const;
    bool isEvent() const;
    bool isError() const;

    QList<MessageData> getEvents() const;
    bool canMerge(const MessageData& other) const;
    void merge(const MessageData& other);
    void initFrom(IrcMessage* message);

    QString format() const;
    void setFormat(const QString& format);

    QString nick() const;
    QByteArray data() const;
    QDateTime timestamp() const;
    IrcMessage::Type type() const;

private:
    struct Private {
        bool own;
        bool error;
        bool reply;
        QString nick;
        QString format;
        QByteArray data;
        QDateTime timestamp;
        IrcMessage::Type type;
        QList<MessageData> events;
    } d;
};

#endif // MESSAGEDATA_H
