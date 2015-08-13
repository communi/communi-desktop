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

#include "commandverifier.h"
#include <IrcConnection>
#include <IrcCommand>
#include <IrcMessage>

int CommandVerifier::Private::id = 1;

CommandVerifier::CommandVerifier(IrcConnection* connection) : QObject(connection)
{
    d.connection = connection;
    connection->installMessageFilter(this);
    connection->installCommandFilter(this);
}

int CommandVerifier::identify(IrcMessage* message) const
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        QMapIterator<int, IrcCommand*> it(d.commands);
        while (it.hasNext()) {
            IrcMessage* cmd = it.next().value()->toMessage(message->prefix(), message->connection());
            if (cmd && cmd->command() == message->command() && cmd->parameters() == message->parameters()) {
                cmd->deleteLater();
                return it.key();
            }
        }
    }
    return 0;
}

bool CommandVerifier::messageFilter(IrcMessage* message)
{
    if (d.commands.isEmpty())
        return false;

    if (message->isOwn() && (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice)) {
        IrcNetwork* network = message->network();
        if (network && network->isCapable("echo-message")) {
            int id = identify(message);
            if (id > 0) {
                IrcCommand* command = d.commands.take(id);
                if (command) {
                    emit verified(id);
                    command->deleteLater();
                    return true;
                }
            }
        }
    } else if (message->type() == IrcMessage::Pong) {
        QString arg = static_cast<IrcPongMessage*>(message)->argument();
        if (arg.startsWith("communi/")) {
            bool ok = false;
            int id = arg.mid(8).toInt(&ok);
            if (ok) {
                IrcCommand* command = d.commands.take(id);
                if (command) {
                    emit verified(id);
                    command->deleteLater();
                    return true;
                }
            }
        }
    }
    return false;
}

bool CommandVerifier::commandFilter(IrcCommand* command)
{
    if (!command->parent() && (command->type() == IrcCommand::Message ||
                               command->type() == IrcCommand::Notice ||
                               command->type() == IrcCommand::CtcpAction)) {
        command->setParent(this); // take ownership
        d.id = qMax(1, d.id + 1); // overflow -> 1
        d.commands.insert(d.id, command);

        IrcConnection* connection = command->connection();
        if (connection) {
            IrcNetwork* network = connection->network();
            if (network && network->isCapable("echo-message"))
                return false;
        }

        d.connection->sendCommand(command);
        d.connection->sendData("PING communi/" + QByteArray::number(d.id));
        return true;
    }
    return false;
}
