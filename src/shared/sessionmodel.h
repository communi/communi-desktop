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

#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include <QHash>
#include <QObject>
#include <QPointer>
#include <IrcMessage>

class Session;
class MessageReceiver;

class SessionModel : public QObject
{
    Q_OBJECT

public:
    explicit SessionModel(QObject* parent = 0);
    virtual ~SessionModel();

    Session* session() const;
    void setSession(Session* session);

    MessageReceiver* defaultReceiver() const;
    void setDefaultReceiver(MessageReceiver* receiver);

    MessageReceiver* currentReceiver() const;
    void setCurrentReceiver(MessageReceiver* receiver);

    void addReceiver(const QString& name, MessageReceiver* receiver);
    void removeReceiver(const QString& name);

public slots:
    void handleMessage(IrcMessage* message);

signals:
    void receiverToBeAdded(const QString& name);
    void receiverToBeRenamed(const QString& from, const QString& to);
    void receiverToBeRemoved(const QString& name);

protected:
    void handleInviteMessage(IrcInviteMessage* message);
    void handleJoinMessage(IrcJoinMessage* message);
    void handleKickMessage(IrcKickMessage* message);
    void handleModeMessage(IrcModeMessage* message);
    void handleNickMessage(IrcNickMessage* message);
    void handleNoticeMessage(IrcNoticeMessage* message);
    void handleNumericMessage(IrcNumericMessage* message);
    void handlePartMessage(IrcPartMessage* message);
    void handlePongMessage(IrcPongMessage* message);
    void handlePrivateMessage(IrcPrivateMessage* message);
    void handleQuitMessage(IrcQuitMessage* message);
    void handleTopicMessage(IrcTopicMessage* message);
    void handleUnknownMessage(IrcMessage* message);

    void sendMessage(IrcMessage* message, MessageReceiver* receiver);
    void sendMessage(IrcMessage* message, const QString& receiver);

private:
    struct Private {
        QPointer<Session> session;
        MessageReceiver* defaultReceiver;
        MessageReceiver* currentReceiver;
        QHash<QString, MessageReceiver*> receivers;
    } d;
};

#endif // SESSIONMODEL_H
