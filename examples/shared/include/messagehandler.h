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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QHash>
#include <QObject>
#include <IrcMessage>

class IrcSession;

class MessageHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool qml READ isQml WRITE setQml)
    Q_PROPERTY(IrcSession* session READ session WRITE setSession)
    Q_PROPERTY(QObject* defaultReceiver READ defaultReceiver WRITE setDefaultReceiver)
    Q_PROPERTY(QObject* currentReceiver READ currentReceiver WRITE setCurrentReceiver)

public:
    explicit MessageHandler(QObject* parent = 0);
    virtual ~MessageHandler();

    bool isQml() const;
    void setQml(bool qml);

    IrcSession* session() const;
    void setSession(IrcSession* session);

    QObject* defaultReceiver() const;
    void setDefaultReceiver(QObject* receiver);

    QObject* currentReceiver() const;
    void setCurrentReceiver(QObject* receiver);

    Q_INVOKABLE void addReceiver(const QString& name, QObject* receiver);
    Q_INVOKABLE QObject* getReceiver(const QString& name) const;
    Q_INVOKABLE void removeReceiver(const QString& name);

public slots:
    void handleMessage(IrcMessage* message);

signals:
    void receiverToBeAdded(const QString& name);
    void receiverToBeRenamed(const QString& from, const QString& to);
    void receiverToBeRemoved(const QString &name);

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

    void sendMessage(IrcMessage* message, QObject* receiver);
    void sendMessage(IrcMessage* message, const QString& receiver);

private:
    struct Private
    {
        QStringList userChannels(const QString& user) const;
        void addChannelUser(const QString& channel, const QString& user);
        void removeChannelUser(const QString& channel, const QString& user);

        bool qml;
        IrcSession* session;
        QObject* defaultReceiver;
        QObject* currentReceiver;
        QHash<QString, QObject*> receivers;
        QHash<QString, QSet<QString> > channelUsers;
    } d;
};

#endif // MESSAGEHANDLER_H
