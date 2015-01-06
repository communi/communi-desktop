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

#include "awayplugin.h"
#include <IrcConnection>
#include <IrcNetwork>
#include <IrcMessage>
#include <IrcCommand>
#include <IrcChannel>
#include <QTimer>
#include <Irc>

AwayPlugin::AwayPlugin(QObject* parent) : QObject(parent)
{
}

void AwayPlugin::connectionAdded(IrcConnection* connection)
{
    connection->installMessageFilter(this);

    IrcNetwork* network = connection->network();
    QStringList caps = network->requestedCapabilities();
    caps += "away-notify";
    network->setRequestedCapabilities(caps);
}

void AwayPlugin::bufferAdded(IrcBuffer* buffer)
{
    IrcChannel* channel = buffer->toChannel();
    if (channel) {
        queueChannel(channel);
        connect(channel, SIGNAL(activeChanged(bool)), this, SLOT(onChannelActiveChanged()));
        connect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(onChannelDestroyed(IrcChannel*)));
    }
}

bool AwayPlugin::messageFilter(IrcMessage* message)
{
    if (message->type() == IrcMessage::Numeric) {
        const int code = static_cast<IrcNumericMessage*>(message)->code();
        if (code == Irc::RPL_WHOREPLY || code == Irc::RPL_ENDOFWHO) {
            const QString mask = message->parameters().value(1);
            foreach (IrcChannel* channel, d.queue) {
                if (channel->connection() == message->connection() && channel->title() == mask) {
                    if (code == Irc::RPL_ENDOFWHO)
                        d.queue.remove(channel);
                    return true;
                }
            }
        }
    }
    return false;
}

void AwayPlugin::onChannelActiveChanged()
{
    queueChannel(qobject_cast<IrcChannel*>(sender()));
}

void AwayPlugin::onChannelDestroyed(IrcChannel* channel)
{
    d.queue.remove(channel);
}

void AwayPlugin::queueChannel(IrcChannel* channel)
{
    if (channel && channel->isActive() && !d.queue.contains(channel)) {
        IrcNetwork* network = channel->network();
        if (network && network->isCapable("away-notify")) {
            QTimer::singleShot(500, channel, SLOT(who()));
            d.queue.insert(channel);
        }
    }
}
