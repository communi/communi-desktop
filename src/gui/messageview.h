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
#include "messagereceiver.h"
#include "messageformatter.h"
#include "settings.h"

class MenuFactory;
class IrcMessage;
class Session;

class MessageView : public QWidget, public MessageReceiver
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(QString receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)

public:
    enum ViewType { ServerView, ChannelView, QueryView };

    MessageView(ViewType type, Session* session, QWidget* parent = 0);
    ~MessageView();

    bool isActive() const;
    ViewType viewType() const;
    Session* session() const;
    UserModel* userModel() const;
    QTextBrowser* textBrowser() const;
    MessageFormatter* messageFormatter() const;

    QString receiver() const;
    void setReceiver(const QString& receiver);

    MenuFactory* menuFactory() const;
    void setMenuFactory(MenuFactory* factory);

    QByteArray saveSplitter() const;
    void restoreSplitter(const QByteArray& state);

public slots:
    void showHelp(const QString& text, bool error = false);
    void sendMessage(const QString& message);
    void appendMessage(const QString& message);
    void applySettings(const Settings& settings);

signals:
    void activeChanged();
    void receiverChanged(const QString& receiver);

    void highlighted(IrcMessage* message);
    void missed(IrcMessage* message);
    void queried(const QString& user);
    void messaged(const QString& user, const QString& message);
    void splitterChanged(const QByteArray& state);

protected:
    void hideEvent(QHideEvent* event);
    bool eventFilter(QObject* object, QEvent* event);

    void receiveMessage(IrcMessage* message);
    bool hasUser(const QString& user) const;

private slots:
    void onEscPressed();
    void onSplitterMoved();
    void onAnchorClicked(const QUrl& link);
    void completeCommand(const QString& command);

private:
    struct MessageViewData : public Ui::MessageView {
        ViewType viewType;
        QString receiver;
        Session* session;
        MessageFormatter* formatter;
        Settings settings;
        QString topic;
        bool joined;
    } d;
};

#endif // MESSAGEVIEW_H
