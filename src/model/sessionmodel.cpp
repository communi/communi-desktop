/*
* Copyright (C) 2008-2013 Communi authors
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

#include "sessionmodel.h"
#include "channelitem.h"
#include "serveritem.h"
#include "queryitem.h"
#include "session.h"

#include <qdebug.h>
#include <irc.h>

SessionModel::SessionModel(QObject* parent) : QObject(parent)
{
    d.server = 0;
    d.current = 0;
    setSession(qobject_cast<Session*>(parent));
}

SessionModel::~SessionModel()
{
    d.server = 0;
    d.current = 0;
    d.channels.clear();
    d.queries.clear();
    d.items.clear();
}

Session* SessionModel::session() const
{
    return d.session;
}

void SessionModel::setSession(Session* session)
{
    if (d.session != session) {
        if (d.session) {
            delete d.server;
            disconnect(d.session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(handleMessage(IrcMessage*)));
        }

        d.session = session;

        if (d.session) {
            d.current = d.server = new ServerItem(this);
            connect(d.session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(handleMessage(IrcMessage*)));
        }
    }
}

ServerItem* SessionModel::server() const
{
    return d.server;
}

QList<ChannelItem*> SessionModel::channels() const
{
    return d.channels;
}

ChannelItem* SessionModel::channel(const QString& name) const
{
    return qobject_cast<ChannelItem*>(d.items.value(name.toLower()));
}

QList<QueryItem*> SessionModel::queries() const
{
    return d.queries;
}

QueryItem* SessionModel::query(const QString& name) const
{
    return qobject_cast<QueryItem*>(d.items.value(name.toLower()));
}

SessionItem* SessionModel::item(const QString& name)
{
    return d.items.value(name.toLower());
}

SessionItem* SessionModel::addItem(const QString& name)
{
    QString lower = name.toLower();
    SessionItem* item = d.items.value(lower);
    if (!item) {
        if (d.session->isChannel(name))
            item = new ChannelItem(name, this);
        else
            item = new QueryItem(name, this);
        emit itemAdded(item);
        d.items.insert(lower, item);
    }
    return item;
}

void SessionModel::removeItem(const QString& name)
{
    SessionItem* item = d.items.take(name.toLower());
    if (item)
        removeItem(item);
}

void SessionModel::removeItem(SessionItem* item)
{
    if (ChannelItem* channel = qobject_cast<ChannelItem*>(item))
        d.channels.removeOne(channel);
    else if (QueryItem* query = qobject_cast<QueryItem*>(item))
        d.queries.removeOne(query);
    emit itemRemoved(item);
    item->deleteLater();
}

SessionItem* SessionModel::currentItem() const
{
    return d.current;
}

void SessionModel::setCurrentItem(SessionItem* item)
{
    d.current = item ? item : d.server;
}

void SessionModel::handleMessage(IrcMessage* message)
{
    switch (message->type()) {
        case IrcMessage::Invite:
            handleInviteMessage(static_cast<IrcInviteMessage*>(message));
            break;
        case IrcMessage::Join:
            handleJoinMessage(static_cast<IrcJoinMessage*>(message));
            break;
        case IrcMessage::Kick:
            handleKickMessage(static_cast<IrcKickMessage*>(message));
            break;
        case IrcMessage::Mode:
            handleModeMessage(static_cast<IrcModeMessage*>(message));
            break;
        case IrcMessage::Nick:
            handleNickMessage(static_cast<IrcNickMessage*>(message));
            break;
        case IrcMessage::Notice:
            handleNoticeMessage(static_cast<IrcNoticeMessage*>(message));
            break;
        case IrcMessage::Numeric:
            handleNumericMessage(static_cast<IrcNumericMessage*>(message));
            break;
        case IrcMessage::Part:
            handlePartMessage(static_cast<IrcPartMessage*>(message));
            break;
        case IrcMessage::Pong:
            handlePongMessage(static_cast<IrcPongMessage*>(message));
            break;
        case IrcMessage::Private:
            handlePrivateMessage(static_cast<IrcPrivateMessage*>(message));
            break;
        case IrcMessage::Quit:
            handleQuitMessage(static_cast<IrcQuitMessage*>(message));
            break;
        case IrcMessage::Topic:
            handleTopicMessage(static_cast<IrcTopicMessage*>(message));
            break;
        case IrcMessage::Unknown:
            handleUnknownMessage(static_cast<IrcMessage*>(message));
            break;
        default:
            break;
    }
}

void SessionModel::handleInviteMessage(IrcInviteMessage* message)
{
    d.current->receiveMessage(message);
}

void SessionModel::handleJoinMessage(IrcJoinMessage* message)
{
    sendMessage(message, message->channel());
}

void SessionModel::handleKickMessage(IrcKickMessage* message)
{
    sendMessage(message, message->channel());
}

void SessionModel::handleModeMessage(IrcModeMessage* message)
{
    if (message->sender().name() == message->target())
        d.server->receiveMessage(message);
    else
        sendMessage(message, message->target());
}

void SessionModel::handleNickMessage(IrcNickMessage* message)
{
    QString nick = message->sender().name().toLower();
    foreach (SessionItem* item, d.items) {
        if (item->hasUser(nick))
            item->receiveMessage(message);
        if (!nick.compare(item->name(), Qt::CaseInsensitive)) {
            SessionItem* query = d.items.take(nick);
            d.items.insert(message->nick(), query);
        }
    }
}

void SessionModel::handleNoticeMessage(IrcNoticeMessage* message)
{
    if (!d.session->isConnected() || message->target() == "*")
        d.server->receiveMessage(message);
    else if (message->target() == d.session->nickName())
        d.current->receiveMessage(message);
    else
        sendMessage(message, message->target());
}

void SessionModel::handleNumericMessage(IrcNumericMessage* message)
{
    if (QByteArray(Irc::toString(message->code())).startsWith("ERR_")) {
        d.current->receiveMessage(message);
        return;
    }

    switch (message->code()) {
        case Irc::RPL_ENDOFWHO:
        case Irc::RPL_WHOREPLY:
        case Irc::RPL_UNAWAY:
        case Irc::RPL_NOWAWAY:
        case Irc::RPL_AWAY:
        case Irc::RPL_WHOISOPERATOR:
        case Irc::RPL_WHOISMODES: // "is using modes"
        case Irc::RPL_WHOISREGNICK: // "is a registered nick"
        case Irc::RPL_WHOISHELPOP: // "is available for help"
        case Irc::RPL_WHOISSPECIAL: // "is identified to services"
        case Irc::RPL_WHOISHOST: // nick is connecting from <...>
        case Irc::RPL_WHOISSECURE: // nick is using a secure connection
        case Irc::RPL_WHOISUSER:
        case Irc::RPL_WHOISSERVER:
        case Irc::RPL_WHOISACCOUNT: // nick user is logged in as
        case Irc::RPL_WHOWASUSER:
        case Irc::RPL_WHOISIDLE:
        case Irc::RPL_WHOISCHANNELS:
        case Irc::RPL_ENDOFWHOIS:
        case Irc::RPL_INVITING:
        case Irc::RPL_VERSION:
        case Irc::RPL_TIME:
            d.current->receiveMessage(message);
            break;

        case Irc::RPL_ENDOFBANLIST:
        case Irc::RPL_ENDOFEXCEPTLIST:
        case Irc::RPL_ENDOFINFO:
        case Irc::RPL_ENDOFINVITELIST:
        case Irc::RPL_ENDOFLINKS:
        case Irc::RPL_ENDOFSTATS:
        case Irc::RPL_ENDOFUSERS:
        case Irc::RPL_ENDOFWHOWAS:
            break; // ignore

        case Irc::RPL_CHANNELMODEIS:
        case Irc::RPL_CHANNEL_URL:
        case Irc::RPL_CREATIONTIME:
        case Irc::RPL_NOTOPIC:
        case Irc::RPL_TOPIC:
        case Irc::RPL_TOPICWHOTIME:
            sendMessage(message, message->parameters().value(1));
            break;

        case Irc::RPL_NAMREPLY: {
            const int count = message->parameters().count();
            ChannelItem* item = channel(message->parameters().value(count - 2));
            if (item)
                item->receiveMessage(message);
            else
                d.current->receiveMessage(message);
            break;
        }

        case Irc::RPL_ENDOFNAMES:
            if (d.items.contains(message->parameters().value(1).toLower()))
                sendMessage(message, message->parameters().value(1));
            break;

        default:
            d.server->receiveMessage(message);
            break;
    }
}

void SessionModel::handlePartMessage(IrcPartMessage* message)
{
    if (SessionItem* item = channel(message->channel()))
        item->receiveMessage(message);
}

void SessionModel::handlePongMessage(IrcPongMessage* message)
{
    d.current->receiveMessage(message);
}

void SessionModel::handlePrivateMessage(IrcPrivateMessage* message)
{
    if (message->isRequest())
        d.current->receiveMessage(message);
    else if (message->target() == d.session->nickName())
        sendMessage(message, message->sender().name());
    else
        sendMessage(message, message->target());
}

void SessionModel::handleQuitMessage(IrcQuitMessage* message)
{
    QString nick = message->sender().name();
    foreach (SessionItem* item, d.items) {
        if (item->hasUser(nick))
            item->receiveMessage(message);
    }
}

void SessionModel::handleTopicMessage(IrcTopicMessage* message)
{
    sendMessage(message, message->channel());
}

void SessionModel::handleUnknownMessage(IrcMessage* message)
{
    d.server->receiveMessage(message);
}

void SessionModel::sendMessage(IrcMessage* message, const QString& receiver)
{
    SessionItem* item = addItem(receiver);
    item->receiveMessage(message);
}
