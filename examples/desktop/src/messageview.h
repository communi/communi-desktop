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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "ui_messageview.h"
#include <ircmessage.h>
#include <irccommand.h>
#include "session.h"
#include "commandparser.h"
#include "messageformatter.h"
#include "messagereceiver.h"

struct Settings;
class UserModel;
class QStringListModel;

class MessageView : public QWidget, public MessageReceiver
{
    Q_OBJECT

public:
    MessageView(const QString& receiver, Session* session, QWidget* parent = 0);
    ~MessageView();

    bool isChannelView() const;

public slots:
    void showHelp(const QString& text, bool error = false);
    void appendMessage(const QString& message);

signals:
    void highlight(MessageView* view, bool on);
    void alert(MessageView* view, bool on);
    void query(const QString& user);

protected:
    bool eventFilter(QObject* receiver, QEvent* event);

    void receiveMessage(IrcMessage* message);
    bool hasUser(const QString& user) const;
    void addUser(const QString& user);
    void addUsers(const QStringList& users);
    void removeUser(const QString& user);
    void renameUser(const QString &from, const QString &to);

private slots:
    void onEscPressed();
    void onSend(const QString& text);
    void applySettings(const Settings& settings);
    void onCustomCommand(const QString& command, const QStringList& params);

private:
    static QString prettyNames(const QStringList& names, int columns);

    struct MessageViewData : public Ui::MessageView
    {
        QString receiver;
        Session* session;
        CommandParser parser;
        MessageFormatter formatter;
        UserModel* userModel;
        static QStringListModel* commandModel;
        QSet<IrcCommand::Type> sentCommands;
    } d;
};

#endif // MESSAGEVIEW_H
