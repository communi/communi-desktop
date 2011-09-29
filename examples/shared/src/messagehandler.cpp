/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#include "messagehandler.h"
#include <qabstractsocket.h>
#include <ircsession.h>
#include <qvariant.h>
#include <qdebug.h>
#include <irc.h>

MessageHandler::MessageHandler(QObject* parent) : QObject(parent)
{
    d.qml = false;
    d.session = 0;
    d.defaultReceiver = 0;
    d.currentReceiver = 0;
    setSession(qobject_cast<IrcSession*>(parent));
}

MessageHandler::~MessageHandler()
{
    d.defaultReceiver = 0;
    d.currentReceiver = 0;
    d.receivers.clear();
    d.channelUsers.clear();
    d.session->socket()->waitForDisconnected(500);
}

bool MessageHandler::isQml() const
{
    return d.qml;
}

void MessageHandler::setQml(bool qml)
{
    d.qml = qml;
}

IrcSession* MessageHandler::session() const
{
    return d.session;
}

void MessageHandler::setSession(IrcSession* session)
{
    if (d.session != session)
    {
        if (d.session)
            disconnect(d.session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(handleMessage(IrcMessage*)));

        if (session)
        {
            session->setParent(this);
            connect(session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(handleMessage(IrcMessage*)));
        }
        d.session = session;
    }
}

QObject* MessageHandler::defaultReceiver() const
{
    return d.defaultReceiver;
}

void MessageHandler::setDefaultReceiver(QObject* receiver)
{
    d.defaultReceiver = receiver;
}

QObject* MessageHandler::currentReceiver() const
{
    return d.currentReceiver;
}

void MessageHandler::setCurrentReceiver(QObject* receiver)
{
    d.currentReceiver = receiver;
}

void MessageHandler::addReceiver(const QString& name, QObject* receiver)
{
    d.receivers.insert(name.toLower(), receiver);
}

QObject* MessageHandler::getReceiver(const QString& name) const
{
    return d.receivers.value(name.toLower());
}

void MessageHandler::removeReceiver(const QString& name)
{
    const QString lower = name.toLower();
    if (d.receivers.contains(lower))
    {
        emit receiverToBeRemoved(name);
        d.receivers.remove(name.toLower());
    }
}

void MessageHandler::handleMessage(IrcMessage* message)
{
    switch (message->type())
    {
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

void MessageHandler::handleInviteMessage(IrcInviteMessage* message)
{
    sendMessage(message, d.currentReceiver);
}

void MessageHandler::handleJoinMessage(IrcJoinMessage* message)
{
    sendMessage(message, message->channel());
    d.addChannelUser(message->channel(), message->sender().name());
}

void MessageHandler::handleKickMessage(IrcKickMessage* message)
{
    sendMessage(message, message->channel());
    d.removeChannelUser(message->channel(), message->user());
}

void MessageHandler::handleModeMessage(IrcModeMessage* message)
{
    if (message->sender().name() == message->target())
        sendMessage(message, d.defaultReceiver);
    else
        sendMessage(message, message->target());
}

void MessageHandler::handleNickMessage(IrcNickMessage* message)
{
    QString nick = message->sender().name().toLower();
    foreach (const QString& channel, d.userChannels(nick))
    {
        sendMessage(message, channel);
        d.removeChannelUser(channel, nick);
        d.addChannelUser(channel, message->nick());
    }
    foreach (const QString& receiver, d.receivers.keys())
    {
        if (!nick.compare(receiver, Qt::CaseInsensitive))
        {
            emit receiverToBeRenamed(receiver, message->nick());
            QObject* object = d.receivers.take(nick);
            d.receivers.insert(nick, object);
            sendMessage(message, object);
        }
    }
}

void MessageHandler::handleNoticeMessage(IrcNoticeMessage* message)
{
    sendMessage(message, d.currentReceiver);
}

void MessageHandler::handleNumericMessage(IrcNumericMessage* message)
{
    switch (message->code())
    {
    case Irc::RPL_ENDOFWHO:
    case Irc::RPL_WHOREPLY:
    case Irc::RPL_UNAWAY:
    case Irc::RPL_NOWAWAY:
    case Irc::RPL_AWAY:
    case Irc::RPL_WHOISOPERATOR:
    case 310: // "is available for help"
    case 320: // "is identified to services"
    case 378: // nick is connecting from <...>
    case 671: // nick is using a secure connection
    case Irc::RPL_WHOISUSER:
    case Irc::RPL_WHOISSERVER:
    case 330: // nick user is logged in as
    case Irc::RPL_WHOWASUSER:
    case Irc::RPL_WHOISIDLE:
    case Irc::RPL_WHOISCHANNELS:
    case Irc::RPL_INVITING:
    case Irc::RPL_VERSION:
    case Irc::RPL_TIME:
        sendMessage(message, d.currentReceiver);
        break;

    case Irc::RPL_ENDOFBANLIST:
    case Irc::RPL_ENDOFEXCEPTLIST:
    case Irc::RPL_ENDOFINFO:
    case Irc::RPL_ENDOFINVITELIST:
    case Irc::RPL_ENDOFLINKS:
    case Irc::RPL_ENDOFMOTD:
    case Irc::RPL_ENDOFSTATS:
    case Irc::RPL_ENDOFUSERS:
    case Irc::RPL_ENDOFWHOIS:
    case Irc::RPL_ENDOFWHOWAS:
        break; // ignore

    case Irc::RPL_ENDOFNAMES:
    case Irc::RPL_CHANNELMODEIS:
    case Irc::RPL_CHANNELURL:
    case Irc::RPL_CHANNELCREATED:
    case Irc::RPL_NOTOPIC:
    case Irc::RPL_TOPIC:
    case Irc::RPL_TOPICSET:
        sendMessage(message, message->parameters().value(1));
        break;

    case Irc::RPL_NAMREPLY: {
        const int count = message->parameters().count();
        const QString channel = message->parameters().value(count - 2);
        const QStringList nicks = message->parameters().value(count - 1).split(" ", QString::SkipEmptyParts);
        foreach (const QString& nick, nicks)
            d.addChannelUser(channel, nick);
        sendMessage(message, channel);
        break;
        }

    default:
        sendMessage(message, d.defaultReceiver);
        break;
    }
}

void MessageHandler::handlePartMessage(IrcPartMessage* message)
{
    d.removeChannelUser(message->channel(), message->sender().name());
    if (message->sender().name() == d.session->nickName())
        removeReceiver(message->channel());
    else
        sendMessage(message, message->channel());
}

void MessageHandler::handlePongMessage(IrcPongMessage* message)
{
    sendMessage(message, d.currentReceiver);
}

void MessageHandler::handlePrivateMessage(IrcPrivateMessage* message)
{
    if (message->isRequest())
        sendMessage(message, d.currentReceiver);
    else if (message->target() == d.session->nickName())
        sendMessage(message, message->sender().name());
    else
        sendMessage(message, message->target());
}

void MessageHandler::handleQuitMessage(IrcQuitMessage* message)
{
    QString nick = message->sender().name();
    foreach (const QString& channel, d.userChannels(nick))
    {
        sendMessage(message, channel);
        d.removeChannelUser(channel, nick);
    }

    if (d.receivers.contains(nick.toLower()))
        sendMessage(message, nick);

    if (nick == d.session->nickName())
        removeReceiver(d.session->host());
}

void MessageHandler::handleTopicMessage(IrcTopicMessage* message)
{
    sendMessage(message, message->channel());
}

void MessageHandler::handleUnknownMessage(IrcMessage* message)
{
    sendMessage(message, d.defaultReceiver);
}

void MessageHandler::sendMessage(IrcMessage* message, QObject* receiver)
{
    if (d.qml)
        // QML: QVariant(QObject*)
        QMetaObject::invokeMethod(receiver, "receiveMessage", Q_ARG(QVariant, QVariant::fromValue((QObject*) message)));
    else
        // C++: IrcMessage*
        QMetaObject::invokeMethod(receiver, "receiveMessage", Q_ARG(IrcMessage*, message));
}

void MessageHandler::sendMessage(IrcMessage* message, const QString& receiver)
{
    if (!d.receivers.contains(receiver.toLower()))
        emit receiverToBeAdded(receiver);
    QObject* object = getReceiver(receiver);
    Q_ASSERT(object);
    sendMessage(message, object);
}

QStringList MessageHandler::Private::userChannels(const QString& user) const
{
    QStringList channels;
    QHash<QString, QSet<QString> >::const_iterator it;
    for (it = channelUsers.constBegin(); it != channelUsers.constEnd(); ++it)
        if (it.value().contains(user.toLower()))
            channels += it.key();
    return channels;
}

void MessageHandler::Private::addChannelUser(const QString& channel, const QString& user)
{
    channelUsers[channel.toLower()].insert(user.toLower());
    QObject* receiver = receivers.value(channel.toLower());
    if (receiver)
    {
        if (qml)
            QMetaObject::invokeMethod(receiver, "addUser", Q_ARG(QVariant, user));
        else
            QMetaObject::invokeMethod(receiver, "addUser", Q_ARG(QString, user));
    }
}

void MessageHandler::Private::removeChannelUser(const QString& channel, const QString& user)
{
    channelUsers[channel.toLower()].remove(user.toLower());
    QObject* receiver = receivers.value(channel.toLower());
    if (receiver)
    {
        if (qml)
            QMetaObject::invokeMethod(receiver, "removeUser", Q_ARG(QVariant, user));
        else
            QMetaObject::invokeMethod(receiver, "removeUser", Q_ARG(QString, user));
    }
}
