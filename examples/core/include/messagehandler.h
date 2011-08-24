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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
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
    Q_PROPERTY(IrcSession* session READ session WRITE setSession)
    Q_PROPERTY(QObject* defaultReceiver READ defaultReceiver WRITE setDefaultReceiver)

public:
    explicit MessageHandler(QObject* parent = 0);
    virtual ~MessageHandler();

    IrcSession* session() const;
    void setSession(IrcSession* session);

    QObject* defaultReceiver() const;
    void setDefaultReceiver(QObject* receiver);

    Q_INVOKABLE void addReceiver(const QString& name, QObject* channel);
    Q_INVOKABLE QObject* getReceiver(const QString& name) const;
    Q_INVOKABLE void removeReceiver(const QString& name);

public slots:
    void handleMessage(IrcMessage* message);

signals:
    void receiverToBeAdded(const QString& name);
    void receiverToBeRenamed(const QString& from, const QString& to);
    void receiverToBeRemoved(const QString &name);

protected:
    bool handleInviteMessage(IrcInviteMessage* message);
    bool handleJoinMessage(IrcJoinMessage* message);
    bool handleKickMessage(IrcKickMessage* message);
    bool handleModeMessage(IrcModeMessage* message);
    bool handleNickMessage(IrcNickMessage* message);
    bool handleNoticeMessage(IrcNoticeMessage* message);
    bool handleNumericMessage(IrcNumericMessage* message);
    bool handlePartMessage(IrcPartMessage* message);
    bool handlePrivateMessage(IrcPrivateMessage* message);
    bool handleQuitMessage(IrcQuitMessage* message);
    bool handleTopicMessage(IrcTopicMessage* message);
    bool handleUnknownMessage(IrcMessage* message);

    void sendMessage(IrcMessage* message, const QString& receiver = QString());

private:
    struct Private
    {
        QStringList userChannels(const QString& user) const;
        void addChannelUser(const QString& channel, const QString& user);
        void removeChannelUser(const QString& channel, const QString& user);

        IrcSession* session;
        QObject* defaultReceiver;
        QHash<QString, QObject*> receivers;
        QHash<QString, QSet<QString> > channelUsers;
    } d;
};

#endif // MESSAGEHANDLER_H
