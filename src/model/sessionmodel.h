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
class QueryItem;
class ServerItem;
class ChannelItem;
class SessionItem;

class SessionModel : public QObject
{
    Q_OBJECT

public:
    explicit SessionModel(QObject* parent = 0);
    virtual ~SessionModel();

    Session* session() const;
    void setSession(Session* session);

    ServerItem* server() const;

    QList<ChannelItem*> channels() const;
    ChannelItem* channel(const QString& name) const;

    QList<QueryItem*> queries() const;
    QueryItem* query(const QString& name) const;

    SessionItem* item(const QString& name);
    SessionItem* addItem(const QString& name);
    void removeItem(const QString& name);

    SessionItem* currentItem() const;
    void setCurrentItem(SessionItem* item);

public slots:
    void handleMessage(IrcMessage* message);

signals:
    void itemAdded(SessionItem* item);
    void itemRemoved(SessionItem* item);
    void currentItemChanged(SessionItem* item);

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

    void sendMessage(IrcMessage* message, const QString& receiver);

private:
    struct Private {
        QPointer<Session> session;
        ServerItem* server;
        QList<ChannelItem*> channels;
        QList<QueryItem*> queries;
        QHash<QString, SessionItem*> items;
        SessionItem* current;
    } d;
};

#endif // SESSIONMODEL_H
