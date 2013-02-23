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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "ui_messageview.h"
#include "messageformatter.h"
#include "settings.h"

class MenuFactory;
class IrcMessage;
class Session;
class SessionItem;

class MessageView : public QWidget
{
    Q_OBJECT

public:
    enum ViewType { ServerView, ChannelView, QueryView };

    MessageView(SessionItem* item, QWidget* parent = 0);
    ~MessageView();

    SessionItem* item() const;
    Session* session() const;

    ViewType viewType() const;
    UserModel* userModel() const;
    QTextBrowser* textBrowser() const;
    MessageFormatter* messageFormatter() const;

    MenuFactory* menuFactory() const;
    void setMenuFactory(MenuFactory* factory);

    QByteArray saveSplitter() const;
    void restoreSplitter(const QByteArray& state);

public slots:
    void showHelp(const QString& text, bool error = false);
    void sendMessage(const QString& message);
    void applySettings(const Settings& settings);

signals:
    void highlighted(IrcMessage* message);
    void missed(IrcMessage* message);
    void queried(const QString& user);
    void messaged(const QString& user, const QString& message);
    void splitterChanged(const QByteArray& state);

protected:
    void hideEvent(QHideEvent* event);
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void receiveMessage(IrcMessage* message);
    void onEscPressed();
    void onSplitterMoved();
    void onAnchorClicked(const QUrl& link);
    void completeCommand(const QString& command);
    void onTopicChanged(const QString& topic);

private:
    struct MessageViewData : public Ui::MessageView {
        SessionItem* item;
        MessageFormatter* formatter;
        Settings settings;
    } d;
};

#endif // MESSAGEVIEW_H
