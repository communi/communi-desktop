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

#include "messagehandler.h"
#include "messageview.h"
#include "zncmanager.h"
#include <qabstractsocket.h>
#include <ircconnection.h>
#include <qvariant.h>
#include <qdebug.h>
#include <irc.h>

MessageHandler::MessageHandler(QObject* parent) : QObject(parent)
{
    d.znc = new ZncManager(this);
    connect(d.znc, SIGNAL(playbackActiveChanged(bool)), this, SLOT(activatePlayback(bool)));
    connect(d.znc, SIGNAL(playbackTargetChanged(QString)), this, SLOT(playbackView(QString)));

    d.defaultView = 0;
    d.currentView = 0;
}

MessageHandler::~MessageHandler()
{
    d.defaultView = 0;
    d.currentView = 0;
    d.views.clear();
}

ZncManager* MessageHandler::znc() const
{
    return d.znc;
}

MessageView* MessageHandler::defaultView() const
{
    return d.defaultView;
}

void MessageHandler::setDefaultView(MessageView* view)
{
    d.defaultView = view;
}

MessageView* MessageHandler::currentView() const
{
    return d.currentView;
}

void MessageHandler::setCurrentView(MessageView* view)
{
    d.currentView = view;
}

void MessageHandler::addView(const QString& name, MessageView* view)
{
    d.views.insert(name.toLower(), view);
}

void MessageHandler::removeView(const QString& name)
{
    const QString lower = name.toLower();
    if (d.views.contains(lower)) {
        d.views.remove(lower);
        emit viewToBeRemoved(name);
    }
}

void MessageHandler::handleMessage(IrcMessage* message)
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
        case IrcMessage::Names:
            handleNamesMessage(static_cast<IrcNamesMessage*>(message));
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
    sendMessage(message, d.currentView);
}

void MessageHandler::handleJoinMessage(IrcJoinMessage* message)
{
    sendMessage(message, message->channel());
}

void MessageHandler::handleKickMessage(IrcKickMessage* message)
{
    sendMessage(message, message->channel());
}

void MessageHandler::handleModeMessage(IrcModeMessage* message)
{
    if (message->isReply() || message->nick() != message->target())
        sendMessage(message, message->target());
    else
        sendMessage(message, d.defaultView);
}

void MessageHandler::handleNamesMessage(IrcNamesMessage* message)
{
    sendMessage(message, message->channel());
}

void MessageHandler::handleNickMessage(IrcNickMessage* message)
{
    QString oldNick = message->oldNick().toLower();
    QString newNick = message->newNick().toLower();
    if (d.znc->isPlaybackActive()) {
        sendMessage(message, d.znc->playbackTarget());
    } else {
        foreach (MessageView* view, d.views) {
            if (view->viewType() == ViewInfo::Query) {
                if (view->hasUser(oldNick) || !newNick.compare(view->receiver(), Qt::CaseInsensitive))
                    view->receiveMessage(message);
            }
            if (!oldNick.compare(view->receiver(), Qt::CaseInsensitive)) {
                emit viewToBeRenamed(view->receiver(), message->nick());
                if (!d.views.contains(newNick)) {
                    MessageView* object = d.views.take(oldNick);
                    d.views.insert(newNick, object);
                }
            }
        }
    }
}

void MessageHandler::handleNoticeMessage(IrcNoticeMessage* message)
{
    QString target = message->target();

    // forward ChanServ's "[#chan] msg" to the appropriate channel
    if (target == message->connection()->nickName() && message->nick() == "ChanServ") {
        const QString msg = message->message();
        if (msg.startsWith("[")) {
            int idx = msg.indexOf("]");
            if (idx != -1) {
                const QString view = msg.mid(1, idx - 1);
                if (d.views.contains(view.toLower()))
                    target = view;
            }
        }
    }

    if (target == "$$*") {
        // global notice
        foreach (MessageView* view, d.views)
            view->receiveMessage(message);
    } else if (!message->connection()->isConnected() || target.isEmpty() || target == "*")
        sendMessage(message, d.defaultView);
    else if (MessageView* view = d.views.value(message->nick().toLower()))
        sendMessage(message, view);
    else if (target == message->connection()->nickName() || target.contains("*"))
        sendMessage(message, d.currentView);
    else
        sendMessage(message, target);
}

void MessageHandler::handleNumericMessage(IrcNumericMessage* message)
{
    if (Irc::codeToString(message->code()).startsWith("ERR_")) {
        sendMessage(message, d.currentView);
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
            sendMessage(message, d.currentView);
            break;

        case Irc::RPL_ENDOFBANLIST:
        case Irc::RPL_ENDOFEXCEPTLIST:
        case Irc::RPL_ENDOFINFO:
        case Irc::RPL_ENDOFINVITELIST:
        case Irc::RPL_ENDOFLINKS:
        case Irc::RPL_ENDOFSTATS:
        case Irc::RPL_ENDOFUSERS:
        case Irc::RPL_ENDOFWHOWAS:
        case Irc::RPL_NOTOPIC:
        case Irc::RPL_TOPIC:
        case Irc::RPL_CHANNELMODEIS:
            break; // ignore

        case Irc::RPL_CHANNEL_URL:
        case Irc::RPL_CREATIONTIME:
        case Irc::RPL_TOPICWHOTIME:
            sendMessage(message, message->parameters().value(1));
            break;

        case Irc::RPL_NAMREPLY: {
            const int count = message->parameters().count();
            const QString channel = message->parameters().value(count - 2);
            MessageView* view = d.views.value(channel.toLower());
            if (view)
                view->receiveMessage(message);
            else if (d.currentView)
                d.currentView->receiveMessage(message);
            break;
        }

        case Irc::RPL_ENDOFNAMES:
            if (d.views.contains(message->parameters().value(1).toLower()))
                sendMessage(message, message->parameters().value(1));
            break;

        default:
            sendMessage(message, d.defaultView);
            break;
    }
}

void MessageHandler::handlePartMessage(IrcPartMessage* message)
{
    MessageView* view = d.views.value(message->channel().toLower());
    if (view)
        sendMessage(message, view);
}

void MessageHandler::handlePongMessage(IrcPongMessage* message)
{
    QString arg = message->argument();
    if (arg.startsWith("_communi_msg_")) {
        arg = arg.mid(13);
        int idx = arg.lastIndexOf("_");
        QString receiver = arg.left(idx);
        int id = arg.mid(idx + 1).toInt();
        if (id > 0 && !receiver.isEmpty()) {
            sendMessage(message, receiver);
            return;
        }
    }
    sendMessage(message, d.currentView);
}

void MessageHandler::handlePrivateMessage(IrcPrivateMessage* message)
{
    if (message->isRequest())
        sendMessage(message, d.currentView);
    else if (message->target() == message->connection()->nickName())
        sendMessage(message, message->nick());
    else
        sendMessage(message, message->target());
}

void MessageHandler::handleQuitMessage(IrcQuitMessage* message)
{
    QString nick = message->nick();
    if (d.znc->isPlaybackActive()) {
        sendMessage(message, d.znc->playbackTarget());
    } else {
        foreach (MessageView* view, d.views) {
            if (view->viewType() == ViewInfo::Query && view->hasUser(nick))
                view->receiveMessage(message);
        }
    }
}

void MessageHandler::handleTopicMessage(IrcTopicMessage* message)
{
    sendMessage(message, message->channel());
}

void MessageHandler::handleUnknownMessage(IrcMessage* message)
{
    sendMessage(message, d.defaultView);
}

void MessageHandler::sendMessage(IrcMessage* message, MessageView* view)
{
    if (view)
        view->receiveMessage(message);
}

void MessageHandler::sendMessage(IrcMessage* message, const QString& receiver)
{
    QString lower = receiver.toLower();
    if (!d.views.contains(lower))
        emit viewToBeAdded(receiver);
    sendMessage(message, d.views.value(lower));
}

void MessageHandler::activatePlayback(bool activate)
{
    MessageView* view = d.views.value(d.znc->playbackTarget().toLower());
    if (view)
        view->setPlaybackMode(activate);
}

void MessageHandler::playbackView(const QString& name)
{
    MessageView* view = d.views.value(name.toLower());
    if (view)
        view->setPlaybackMode(d.znc->isPlaybackActive());
}
