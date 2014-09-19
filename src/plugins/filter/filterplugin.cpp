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
#include <IrcCommand>
#include <Irc>

static const int SILENCE_PERIOD = 30 * 60;

FilterPlugin::FilterPlugin(QObject* parent) : QObject(parent)
{
}

void FilterPlugin::connectionAdded(IrcConnection* connection)
{
    connection->installCommandFilter(this);
    connection->installMessageFilter(this);
}

void FilterPlugin::connectionRemoved(IrcConnection* connection)
{
    connection->removeCommandFilter(this);
    connection->removeMessageFilter(this);
}

bool FilterPlugin::commandFilter(IrcCommand* command)
{
    d.sentCommands.insert(command->type(), qMakePair(QDateTime::currentDateTime(), command->parameters().value(0)));
    return false;
}

bool FilterPlugin::messageFilter(IrcMessage* message)
{
    if (message->type() == IrcMessage::Numeric) {
        int code = static_cast<IrcNumericMessage*>(message)->code();
        if (code == Irc::RPL_AWAY) {
            QPair<QDateTime, QString> reply = d.awayReplies.value(message->prefix());
            bool filter = reply.first.secsTo(message->timeStamp()) < SILENCE_PERIOD && reply.second == message->parameters().last();
            if (!filter || !d.awayReplies.contains(message->prefix()))
                d.awayReplies.insert(message->prefix(), qMakePair(message->timeStamp(), message->parameters().last()));
            return filter;
        } else if (code == Irc::RPL_TOPIC || code == Irc::RPL_NOTOPIC) {
            QPair<QDateTime, QString> command = d.sentCommands.value(IrcCommand::Topic);
            QString channel = message->parameters().value(1);
            return !command.first.isValid() ||
                    command.first.secsTo(QDateTime::currentDateTime()) >= 120 ||
                    command.second.compare(channel, Qt::CaseInsensitive) != 0;
        } else if (code == Irc::RPL_NAMREPLY || code == Irc::RPL_ENDOFNAMES) {
            QPair<QDateTime, QString> command = d.sentCommands.value(IrcCommand::Names);
            QString channel = message->parameters().value(code == Irc::RPL_NAMREPLY ? 2 : 1);
            return !command.first.isValid() ||
                    command.first.secsTo(QDateTime::currentDateTime()) >= 120 ||
                    command.second.compare(channel, Qt::CaseInsensitive) != 0;
        }
    }
    return false;
}
