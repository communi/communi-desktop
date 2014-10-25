/*
  Copyright (C) 2008-2014 The Communi Project

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
