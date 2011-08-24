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
    }
}

void MessageHandler::handleInviteMessage(IrcInviteMessage* message)
{
}

void MessageHandler::handleJoinMessage(IrcJoinMessage* message)
{
    sendMessage(message->channel(), message);
    d.addChannelUser(message->channel(), IrcPrefix(message->prefix()).nick());
}

void MessageHandler::handleKickMessage(IrcKickMessage* message)
{
    d.removeChannelUser(message->channel(), message->user());
}

void MessageHandler::handleModeMessage(IrcModeMessage* message)
{
}

void MessageHandler::handleNickMessage(IrcNickMessage* message)
{
    QString nick = IrcPrefix(message->prefix()).nick();
    foreach (const QString& channel, d.userChannels(nick))
    {
        d.removeChannelUser(channel, nick);
        d.addChannelUser(channel, message->nick());
    }
}

void MessageHandler::handleNoticeMessage(IrcNoticeMessage* message)
{
}

void MessageHandler::handleNumericMessage(IrcNumericMessage* message)
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
}

void MessageHandler::handlePartMessage(IrcPartMessage* message)
{
    d.removeChannelUser(message->channel(), IrcPrefix(message->prefix()).nick());
}

void MessageHandler::handlePrivateMessage(IrcPrivateMessage* message)
{
    sendMessage(message->target(), message);
}

void MessageHandler::handleQuitMessage(IrcQuitMessage* message)
{
    QString nick = IrcPrefix(message->prefix()).nick();
    foreach (const QString& channel, d.userChannels(nick))
        d.removeChannelUser(channel, nick);
}

void MessageHandler::handleTopicMessage(IrcTopicMessage* message)
{
}

void MessageHandler::handleUnknownMessage(IrcMessage* message)
{
}

void MessageHandler::sendMessage(const QString& receiver, IrcMessage* message)
{
    if (!d.receivers.contains(receiver.toLower()))
        emit receiverToBeAdded(receiver);
    QObject* object = getReceiver(receiver);
    Q_ASSERT(object);
    // TODO: handle parameter type
    //       - C++: IrcMessage*
    //       - QML: QVariant
    qDebug() << QMetaObject::invokeMethod(object, "receiveMessage", Q_ARG(QVariant, QVariant::fromValue((QObject*)message)));
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
