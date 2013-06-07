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

#include "session.h"
#include "application.h"
#include "settingsmodel.h"
#include <IrcCommand>
#include <IrcMessage>
#include <Irc>

Session::Session(QObject* parent) : IrcSession(parent)
{
    d.rejoin = 10;
    d.quit = false;
    d.timestamp = 0;
    d.timestamper.invalidate();

    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(onDisconnected()));
    connect(this, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
    connect(this, SIGNAL(capabilities(QStringList, QStringList*)), this, SLOT(onCapabilities(QStringList, QStringList*)));

    connect(&d.reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnect()));

    applySettings();
    connect(Application::settings(), SIGNAL(changed()), this, SLOT(applySettings()));
}

Session::~Session()
{
}

QString Session::name() const
{
    return d.name;
}

void Session::setName(const QString& name)
{
    if (d.name != name) {
        d.name = name;
        emit nameChanged(name);
    }
}

int Session::autoReconnectDelay() const
{
    return d.reconnectTimer.interval() / 1000;
}

void Session::setAutoReconnectDelay(int delay)
{
    d.reconnectTimer.setInterval(qMax(0, delay) * 1000);
}

bool Session::isChannel(const QString& receiver) const
{
    IrcSessionInfo info(this);
    return receiver.length() > 1 && info.channelTypes().contains(receiver.at(0));
}

QString Session::password() const
{
    return d.password;
}

void Session::setPassword(const QString& password)
{
    d.password = password;
}

bool Session::hasQuit() const
{
    return d.quit;
}

void Session::setHasQuit(bool quit)
{
    d.quit = quit;
}

bool Session::isReconnecting() const
{
    return d.reconnectTimer.isActive();
}

ViewInfos Session::views() const
{
    return d.views;
}

void Session::setViews(const ViewInfos& views)
{
    d.views = views;
}

bool Session::sendUiCommand(IrcCommand* command, const QString& identifier)
{
//    TODO:
//    if (command->type() == IrcCommand::Join) {
//        QString key = command->parameters().value(1);
//        if (!key.isEmpty())
//            setChannelKey(command->parameters().value(0), key);
//    }
    d.commands.insert(identifier, command);
    command->setParent(this); // take ownership
    return sendCommand(command) &&
           sendCommand(IrcCommand::createPing(identifier));
}

void Session::reconnect()
{
    d.quit = false;
    if (!isActive()) {
        d.reconnectTimer.stop();
        open();
    }
}

void Session::quit()
{
    sleep();
    d.quit = true;
}

void Session::destructLater()
{
    if (isConnected()) {
        connect(this, SIGNAL(disconnected()), SLOT(deleteLater()));
        connect(this, SIGNAL(socketError(QAbstractSocket::SocketError)), SLOT(deleteLater()));
        QTimer::singleShot(1000, this, SLOT(deleteLater()));
    } else {
        deleteLater();
    }
}

void Session::stopReconnecting()
{
    d.reconnectTimer.stop();
}

void Session::sleep()
{
    QString message = tr("%1 %2").arg(QApplication::applicationName())
                                 .arg(QApplication::applicationVersion());

    if (isConnected()) {
        IrcSessionInfo info(this);
        if (info.activeCapabilities().contains("communi"))
            sendCommand(IrcCommand::createCtcpRequest("*communi", "TIME"));
        sendCommand(IrcCommand::createQuit(message));
    } else {
        close();
    }
}

void Session::wake()
{
    if (!d.quit)
        reconnect();
}

void Session::onConnected()
{
    if (d.rejoin > 0) {
        QStringList channels;
        foreach (const ViewInfo& view, d.views) {
            if (view.active && view.type == ViewInfo::Channel)
                channels += view.name;
        }
        // send join commands in batches of max N channels
        while (!channels.isEmpty()) {
            sendCommand(IrcCommand::createJoin(QStringList(channels.mid(0, d.rejoin)).join(",")));
            channels = channels.mid(d.rejoin);
        }
    }

    // send pending commands
    QHash<QString, IrcCommand*> cmds = d.commands;
    d.commands.clear();
    foreach (IrcCommand* cmd, cmds)
        sendUiCommand(cmd, cmds.key(cmd));

    d.timestamper.invalidate();
}

void Session::onDisconnected()
{
    if (!d.quit && !d.reconnectTimer.isActive() && d.reconnectTimer.interval() > 0)
        d.reconnectTimer.start();
}

void Session::onPassword(QString* password)
{
    *password = d.password;
}

void Session::onCapabilities(const QStringList& available, QStringList* request)
{
    if (available.contains("identify-msg"))
        request->append("identify-msg");
    if (available.contains("communi")) {
        request->append("communi");
        request->append(QString("communi/%1").arg(d.timestamp));
    }
}

void Session::applySettings()
{
    SettingsModel* settings = Application::settings();
    setAutoReconnectDelay(settings->value("session.reconnectDelay").toInt());
    d.rejoin = 0;
    if (settings->value("ui.rememberChannels").toBool())
        d.rejoin = settings->value("session.channelRejoinBatch").toInt();
}

bool Session::messageFilter(IrcMessage* message)
{
    if (d.timestamp > 0 && d.timestamper.isValid()) {
        long elapsed = d.timestamper.elapsed() / 1000;
        if (elapsed > 0) {
            d.timestamp += elapsed;
            d.timestamper.restart();
        }
    }

    if (message->type() == IrcMessage::Join) {
        if (message->flags() & IrcMessage::Own)
            addChannel(static_cast<IrcJoinMessage*>(message)->channel());
    } else if (message->type() == IrcMessage::Part) {
        if (message->flags() & IrcMessage::Own)
            removeChannel(static_cast<IrcPartMessage*>(message)->channel());
    } else if (message->type() == IrcMessage::Numeric) {
        int code = static_cast<IrcNumericMessage*>(message)->code();
        if (code == Irc::ERR_NICKNAMEINUSE) {
            if (d.alternateNicks.isEmpty()) {
                QString currentNick = nickName();
                d.alternateNicks << (currentNick + "_")
                                 <<  currentNick
                                 << (currentNick + "__")
                                 <<  currentNick;
            }
            setNickName(d.alternateNicks.takeFirst());
        }
    } else if (message->type() == IrcMessage::Notice) {
        if (message->sender().name() == "*communi") {
            d.timestamp = static_cast<IrcNoticeMessage*>(message)->message().toLong();
            d.timestamper.restart();
            return true;
        }
    } else if (message->type() == IrcMessage::Pong) {
        QString identifier = static_cast<IrcPongMessage*>(message)->argument();
        delete d.commands.take(identifier);
    }
    return false;
}

void Session::addChannel(const QString& channel)
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

void Session::removeChannel(const QString& channel)
{
    for (int i = 0; i < d.views.count(); ++i) {
        ViewInfo view = d.views.at(i);
        if (view.type == ViewInfo::Channel && !view.name.compare(channel, Qt::CaseInsensitive)) {
            d.views.removeAt(i);
            return;
        }
    }
}
