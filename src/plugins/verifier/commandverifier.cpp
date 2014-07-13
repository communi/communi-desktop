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
    if (message->type() == IrcMessage::Private) {
        QMapIterator<int, IrcCommand*> it(d.commands);
        while (it.hasNext()) {
            IrcMessage* cmd = it.next().value()->toMessage(message->prefix(), message->connection());
            if (cmd) {
                cmd->deleteLater();
                if (cmd->toData() == QString::fromUtf8(message->toData()))
                    return it.key();
            }
        }
    }
    return 0;
}

bool CommandVerifier::messageFilter(IrcMessage* message)
{
    if (message->type() == IrcMessage::Pong) {
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
    if (command->type() == IrcCommand::Message) {
        command->setParent(this); // take ownership
        d.id = qMax(1, d.id + 1); // overflow -> 1
        d.commands.insert(d.id, command);
        d.connection->sendCommand(command);
        d.connection->sendData("PING communi/" + QByteArray::number(d.id));
        return true;
    }
    return false;
}
