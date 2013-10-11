/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "connection.h"
#include "application.h"
#include <IrcCommand>
#include <IrcMessage>
#include <Irc>

Connection::Connection(QObject* parent) : IrcConnection(parent)
{
    d.bouncer = false;

    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(nickNameReserved(QString*)), this, SLOT(onNickNameReserved(QString*)));

    setReconnectDelay(15);
}

Connection::~Connection()
{
}

bool Connection::isBouncer() const
{
    return d.bouncer;
}

ViewInfos Connection::views() const
{
    return d.views;
}

void Connection::setViews(const ViewInfos& views)
{
    d.views = views;
}

bool Connection::sendUiCommand(IrcCommand* command, const QString& identifier)
{
//    TODO:
//    if (command->type() == IrcCommand::Join) {
//        QString key = command->parameters().value(1);
//        if (!key.isEmpty())
//            setChannelKey(command->parameters().value(0), key);
//    }
    if (!identifier.isEmpty()) {
        d.commands.insert(identifier, command);
        command->setParent(this); // take ownership
        return sendCommand(command) &&
               sendCommand(IrcCommand::createPing(identifier));
    }
    if (command->type() == IrcCommand::Quit)
        setDisabled(true);
    return sendCommand(command);
}

void Connection::quit()
{
    QString message = tr("%1 %2 - http://%3").arg(QApplication::applicationName())
                                             .arg(QApplication::applicationVersion())
                                             .arg(QApplication::organizationDomain());

    if (isConnected()) {
        if (network()->activeCapabilities().contains("communi"))
            sendCommand(IrcCommand::createCtcpRequest("*communi", "TIME"));
        sendCommand(IrcCommand::createQuit(message));
    }
}

IrcCommand* Connection::createCtcpReply(IrcPrivateMessage* request) const
{
    const QString cmd = request->message().split(" ", QString::SkipEmptyParts).value(0).toUpper();
    if (cmd == "VERSION") {
        const QString message = tr("%1 %2").arg(QApplication::applicationName())
                                           .arg(QApplication::applicationVersion());
        return IrcCommand::createCtcpReply(request->nick(), QString("VERSION %1").arg(message));
    }
    return IrcConnection::createCtcpReply(request);
}

void Connection::onConnected()
{
    if (!d.bouncer) {
        QStringList channels;
        foreach (const ViewInfo& view, d.views) {
            if (view.active && view.type == ViewInfo::Channel)
                channels += view.name;
        }
        // send join commands in batches of max N channels
        while (!channels.isEmpty()) {
            sendCommand(IrcCommand::createJoin(QStringList(channels.mid(0, 10)).join(",")));
            channels = channels.mid(10);
        }
    }

    // send pending commands
    QHash<QString, IrcCommand*> cmds = d.commands;
    d.commands.clear();
    foreach (IrcCommand* cmd, cmds)
        sendUiCommand(cmd, cmds.key(cmd));
}

void Connection::onNickNameReserved(QString* alternate)
{
    if (d.alternateNicks.isEmpty()) {
        QString currentNick = nickName();
        d.alternateNicks << (currentNick + "_")
                         <<  currentNick
                         << (currentNick + "__")
                         <<  currentNick;
    }
    *alternate = d.alternateNicks.takeFirst();
}

bool Connection::messageFilter(IrcMessage* message)
{
    if (message->type() == IrcMessage::Capability) {
        d.bouncer = message->nick() == "irc.znc.in";
    } else if (message->type() == IrcMessage::Join) {
        if (message->flags() & IrcMessage::Own)
            addChannel(static_cast<IrcJoinMessage*>(message)->channel());
    } else if (message->type() == IrcMessage::Part) {
        if (message->flags() & IrcMessage::Own)
            removeChannel(static_cast<IrcPartMessage*>(message)->channel());
    } else if (message->type() == IrcMessage::Pong) {
        QString identifier = static_cast<IrcPongMessage*>(message)->argument();
        delete d.commands.take(identifier);
    }
    return false;
}

void Connection::addChannel(const QString& channel)
{
    foreach (const ViewInfo& view, d.views) {
        if (view.type == ViewInfo::Channel && !view.name.compare(channel, Qt::CaseInsensitive))
            return;
    }

    ViewInfo view;
    view.active = true;
    view.name = channel;
    view.type = ViewInfo::Channel;
    d.views += view;
}

void Connection::removeChannel(const QString& channel)
{
    for (int i = 0; i < d.views.count(); ++i) {
        ViewInfo view = d.views.at(i);
        if (view.type == ViewInfo::Channel && !view.name.compare(channel, Qt::CaseInsensitive)) {
            d.views.removeAt(i);
            return;
        }
    }
}
