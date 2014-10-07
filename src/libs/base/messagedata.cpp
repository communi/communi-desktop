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

#include "messagedata.h"

MessageData::MessageData()
{
    d.own = false;
    d.error = false;
    d.type = IrcMessage::Unknown;
}

bool MessageData::isEmpty() const
{
    return d.format.isEmpty();
}

bool MessageData::isEvent() const
{
    return d.type == IrcMessage::Join ||
           d.type == IrcMessage::Kick ||
           d.type == IrcMessage::Mode ||
           d.type == IrcMessage::Nick ||
           d.type == IrcMessage::Part ||
           d.type == IrcMessage::Quit ||
           d.type == IrcMessage::Topic;
}

bool MessageData::isError() const
{
    return d.error || d.type == IrcMessage::Error;
}

QList<MessageData> MessageData::getEvents() const
{
    QList<MessageData> events = d.events;
    if (events.isEmpty())
        events += *this;
    return events;
}

bool MessageData::canMerge(const MessageData& other) const
{
    return isEvent() && (!d.own || d.type != IrcMessage::Join)
           && other.isEvent() && (!other.d.own || other.d.type != IrcMessage::Join);
}

void MessageData::merge(const MessageData& other)
{
    d.events = other.getEvents() << *this;
}

void MessageData::initFrom(IrcMessage* message)
{
    d.timestamp = message->timeStamp();
    d.data = message->toData();
    d.nick = message->nick();
    d.type = message->type();
    d.own = message->isOwn();

    if (message->type() == IrcMessage::Quit) {
        QString reason = static_cast<IrcQuitMessage*>(message)->reason();
        if (reason.contains("Ping timeout")
                || reason.contains("Connection reset by peer")
                || reason.contains("Remote host closed the connection")) {
            d.error = true;
        }
    }
}

QString MessageData::format() const
{
    return d.format;
}

void MessageData::setFormat(const QString& format)
{
    d.format = format;
}

QString MessageData::nick() const
{
    return d.nick;
}

QByteArray MessageData::data() const
{
    return d.data;
}

QDateTime MessageData::timestamp() const
{
    return d.timestamp;
}

IrcMessage::Type MessageData::type() const
{
    return d.type;
}
