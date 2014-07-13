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

#include "filterplugin.h"
#include <IrcConnection>
#include <IrcMessage>
#include <Irc>

static const int SILENCE_PERIOD = 30 * 60;

FilterPlugin::FilterPlugin(QObject* parent) : QObject(parent)
{
}

void FilterPlugin::connectionAdded(IrcConnection* connection)
{
    connection->installMessageFilter(this);
}

void FilterPlugin::connectionRemoved(IrcConnection* connection)
{
    connection->removeMessageFilter(this);
}

bool FilterPlugin::messageFilter(IrcMessage* message)
{
    if (message->type() == IrcMessage::Numeric) {
        if (static_cast<IrcNumericMessage*>(message)->code() == Irc::RPL_AWAY) {
            QPair<QDateTime, QString> reply = d.awayReplies.value(message->prefix());
            bool filter = reply.first.secsTo(message->timeStamp()) < SILENCE_PERIOD && reply.second == message->parameters().last();
            if (!filter || !d.awayReplies.contains(message->prefix()))
                d.awayReplies.insert(message->prefix(), qMakePair(message->timeStamp(), message->parameters().last()));
            return filter;
        }
    }
    return false;
}
