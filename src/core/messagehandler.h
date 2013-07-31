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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QHash>
#include <QObject>
#include <QPointer>
#include <IrcMessage>

class ZncManager;
class MessageView;

class MessageHandler : public QObject
{
    Q_OBJECT

public:
    explicit MessageHandler(QObject* parent = 0);
    virtual ~MessageHandler();

    ZncManager* znc() const;

    MessageView* defaultView() const;
    void setDefaultView(MessageView* view);

    MessageView* currentView() const;
    void setCurrentView(MessageView* view);

    void addView(const QString& name, MessageView* view);
    void removeView(const QString& name);

public slots:
    void handleMessage(IrcMessage* message);

signals:
    void viewToBeAdded(const QString& name);
    void viewToBeRenamed(const QString& from, const QString& to);
    void viewToBeRemoved(const QString& name);

protected:
    void handleInviteMessage(IrcInviteMessage* message);
    void handleJoinMessage(IrcJoinMessage* message);
    void handleKickMessage(IrcKickMessage* message);
    void handleModeMessage(IrcModeMessage* message);
    void handleNamesMessage(IrcNamesMessage* message);
    void handleNickMessage(IrcNickMessage* message);
    void handleNoticeMessage(IrcNoticeMessage* message);
    void handleNumericMessage(IrcNumericMessage* message);
    void handlePartMessage(IrcPartMessage* message);
    void handlePongMessage(IrcPongMessage* message);
    void handlePrivateMessage(IrcPrivateMessage* message);
    void handleQuitMessage(IrcQuitMessage* message);
    void handleTopicMessage(IrcTopicMessage* message);
    void handleUnknownMessage(IrcMessage* message);

    void sendMessage(IrcMessage* message, MessageView* view);
    void sendMessage(IrcMessage* message, const QString& receiver);

private slots:
    void activatePlayback(bool activate);
    void playbackView(const QString& view);

private:
    struct Private {
        ZncManager* znc;
        MessageView* defaultView;
        MessageView* currentView;
        QHash<QString, MessageView*> views;
    } d;
};

#endif // MESSAGEHANDLER_H
