/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "messagehandler.h"
#include <ircsession.h>
#include <ircprefix.h>
#include <qvariant.h>
#include <qdebug.h>

MessageHandler::MessageHandler(QObject* parent) : QObject(parent)
{
    d.session = 0;
    d.defaultReceiver = 0;
    d.currentReceiver = 0;
    setSession(qobject_cast<IrcSession*>(parent));
}

MessageHandler::~MessageHandler()
{
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
            connect(session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(handleMessage(IrcMessage*)));
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
    d.receivers.remove(name.toLower());
}

void MessageHandler::handleMessage(IrcMessage* message)
{
    bool handled = false;
    switch (message->type())
    {
    case IrcMessage::Invite:
        handled = handleInviteMessage(static_cast<IrcInviteMessage*>(message));
        break;
    case IrcMessage::Join:
        handled = handleJoinMessage(static_cast<IrcJoinMessage*>(message));
        break;
    case IrcMessage::Kick:
        handled = handleKickMessage(static_cast<IrcKickMessage*>(message));
        break;
    case IrcMessage::Mode:
        handled = handleModeMessage(static_cast<IrcModeMessage*>(message));
        break;
    case IrcMessage::Nick:
        handled = handleNickMessage(static_cast<IrcNickMessage*>(message));
        break;
    case IrcMessage::Notice:
        handled = handleNoticeMessage(static_cast<IrcNoticeMessage*>(message));
        break;
    case IrcMessage::Numeric:
        handled = handleNumericMessage(static_cast<IrcNumericMessage*>(message));
        break;
    case IrcMessage::Part:
        handled = handlePartMessage(static_cast<IrcPartMessage*>(message));
        break;
    case IrcMessage::Private:
        handled = handlePrivateMessage(static_cast<IrcPrivateMessage*>(message));
        break;
    case IrcMessage::Quit:
        handled = handleQuitMessage(static_cast<IrcQuitMessage*>(message));
        break;
    case IrcMessage::Topic:
        handled = handleTopicMessage(static_cast<IrcTopicMessage*>(message));
        break;
    case IrcMessage::Unknown:
        handled = handleUnknownMessage(static_cast<IrcMessage*>(message));
        break;
    }
    if (!handled)
        sendMessage(message, d.defaultReceiver);
}

bool MessageHandler::handleInviteMessage(IrcInviteMessage* message)
{
    sendMessage(message, d.currentReceiver);
    return true;
}

bool MessageHandler::handleJoinMessage(IrcJoinMessage* message)
{
    sendMessage(message, message->channel());
    d.addChannelUser(message->channel(), IrcPrefix(message->prefix()).nick());
    return true;
}

bool MessageHandler::handleKickMessage(IrcKickMessage* message)
{
    sendMessage(message, message->channel());
    d.removeChannelUser(message->channel(), message->user());
    return true;
}

bool MessageHandler::handleModeMessage(IrcModeMessage* message)
{
    sendMessage(message, message->target());
    return true;
}

bool MessageHandler::handleNickMessage(IrcNickMessage* message)
{
    QString nick = IrcPrefix(message->prefix()).nick();
    foreach (const QString& channel, d.userChannels(nick))
    {
        sendMessage(message, channel);
        d.removeChannelUser(channel, nick);
        d.addChannelUser(channel, message->nick());
    }
    return true;
}

bool MessageHandler::handleNoticeMessage(IrcNoticeMessage* message)
{
    sendMessage(message, message->target());
    return true;
}

bool MessageHandler::handleNumericMessage(IrcNumericMessage* message)
{
    switch (message->code())
    {
    case Irc::RPL_NAMREPLY: {
        QString channel = message->parameters().value(1);
        QStringList nicks = message->parameters().value(2).split(" ", QString::SkipEmptyParts);
        foreach (const QString& nick, nicks)
            d.addChannelUser(channel, nick);
        }
        break;
    }
    return false;
}

bool MessageHandler::handlePartMessage(IrcPartMessage* message)
{
    sendMessage(message, message->channel());
    d.removeChannelUser(message->channel(), IrcPrefix(message->prefix()).nick());
    return true;
}

bool MessageHandler::handlePrivateMessage(IrcPrivateMessage* message)
{
    sendMessage(message, message->target());
    return true;
}

bool MessageHandler::handleQuitMessage(IrcQuitMessage* message)
{
    QString nick = IrcPrefix(message->prefix()).nick();
    foreach (const QString& channel, d.userChannels(nick))
    {
        sendMessage(message, channel);
        d.removeChannelUser(channel, nick);
    }
    return true;
}

bool MessageHandler::handleTopicMessage(IrcTopicMessage* message)
{
    sendMessage(message, message->channel());
    return true;
}

bool MessageHandler::handleUnknownMessage(IrcMessage* message)
{
    return false;
}

void MessageHandler::sendMessage(IrcMessage* message, QObject* receiver)
{
    // TODO: handle parameter type
    //       - C++: IrcMessage*
    //       - QML: QVariant
    QMetaObject::invokeMethod(receiver, "receiveMessage", Q_ARG(QVariant, QVariant::fromValue((QObject*)message)));
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
}

void MessageHandler::Private::removeChannelUser(const QString& channel, const QString& user)
{
    channelUsers[channel.toLower()].remove(user.toLower());
}
