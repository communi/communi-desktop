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
        QString nick;
        QString format;
        QByteArray data;
        QDateTime timestamp;
        IrcMessage::Type type;
        QList<MessageData> events;
    } d;
};

#endif // MESSAGEDATA_H
