/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "ui_messageview.h"
#include "messagereceiver.h"
#include "messageformatter.h"
#include "commandparser.h"
#include "settings.h"

class IrcMessage;
class UserModel;
class Session;

class MessageView : public QWidget, public MessageReceiver
{
    Q_OBJECT

public:
    enum ViewType { ServerView, ChannelView, QueryView };

    MessageView(ViewType type, Session* session, QWidget* parent = 0);
    ~MessageView();

    ViewType viewType() const;

public slots:
    void showHelp(const QString& text, bool error = false);
    void appendMessage(const QString& message);
    void applySettings(const Settings& settings);

signals:
    void highlighted(IrcMessage* message);
    void alerted(IrcMessage* message);
    void queried(const QString& user);

protected:
    bool eventFilter(QObject* receiver, QEvent* event);

    void receiveMessage(IrcMessage* message);
    bool hasUser(const QString& user) const;

private slots:
    void onEscPressed();
    void onSend(const QString& text);
    void onCustomCommand(const QString& command, const QStringList& params);
    void onDoubleClicked(const QModelIndex& index);

private:
    struct MessageViewData : public Ui::MessageView
    {
        ViewType viewType;
        QList<int> receivedCodes;
        Session* session;
        CommandParser parser;
        MessageFormatter formatter;
        UserModel* userModel;
        Settings settings;
    } d;
};

#endif // MESSAGEVIEW_H
