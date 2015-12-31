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

#include "messagedata.h"

MessageData::MessageData()
{
    d.own = false;
    d.error = false;
    d.reply = false;
    d.type = IrcMessage::Unknown;
}

IrcMessage::Type MessageData::effectiveType(const IrcMessage* msg)
{
    QString intent = msg->tag("intent").toString();
    if (!intent.isEmpty()) {
        if (intent == "JOIN")
            return IrcMessage::Join;
        if (intent == "PART")
            return IrcMessage::Part;
        if (intent == "QUIT")
            return IrcMessage::Quit;
        if (intent == "NICK")
            return IrcMessage::Nick;
        if (intent == "MODE")
            return IrcMessage::Mode;
        if (intent == "TOPIC")
            return IrcMessage::Topic;
        if (intent == "KICK")
            return IrcMessage::Kick;
    }
    return msg->type();
}

bool MessageData::isEmpty() const
{
    return d.format.isEmpty();
}

bool MessageData::isEvent() const
{
    return !d.reply &&
           (d.type == IrcMessage::Join ||
            d.type == IrcMessage::Kick ||
            d.type == IrcMessage::Mode ||
            d.type == IrcMessage::Nick ||
            d.type == IrcMessage::Part ||
            d.type == IrcMessage::Quit ||
            d.type == IrcMessage::Topic);
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
           && other.isEvent() && (!other.d.own || other.d.type != IrcMessage::Join)
           && d.timestamp.date() == other.d.timestamp.date();
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
    d.type = effectiveType(message);
    d.own = message->isOwn();
    d.reply = message->property("reply").toBool();

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
