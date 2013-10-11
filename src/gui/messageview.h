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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "ui_messageview.h"
#include "viewinfo.h"
#include <QPointer>
#include <QElapsedTimer>

class SyntaxHighlighter;
class MessageStackView;
class CommandParser;
class IrcMessage;
class IrcBuffer;
class Connection;

class MessageView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(QString receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)

public:
    MessageView(ViewInfo::Type type, IrcConnection* connection, MessageStackView* stack);
    ~MessageView();

    bool isActive() const;
    ViewInfo::Type viewType() const;
    IrcConnection* connection() const;
    Completer* completer() const;
    QTextBrowser* textBrowser() const;

    QString receiver() const;
    void setReceiver(const QString& receiver);

    IrcBuffer* buffer() const;
    void setBuffer(IrcBuffer* buffer);

    bool playbackMode() const;
    void setPlaybackMode(bool enabled);

    QByteArray saveSplitter() const;
    void restoreSplitter(const QByteArray& state);

    bool hasUser(const QString& user) const;

public slots:
    void showHelp(const QString& text, bool error = false);
    void sendMessage(const QString& message);
    void receiveMessage(IrcMessage* message);
    void updateLag(qint64 lag);

signals:
    void activeChanged();
    void receiverChanged(const QString& receiver);

    void highlighted(IrcMessage* message);
    void missed(IrcMessage* message);
    void queried(const QString& user);
    void messaged(const QString& user, const QString& message);
    void splitterChanged(const QByteArray& state);
    void renamed(const QString& from, const QString& to);

protected:
    void hideEvent(QHideEvent* event);
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void onConnected();
    void onDisconnected();
    void onEscPressed();
    void onSplitterMoved();
    void onTitleChanged(const QString& title);
    void onAnchorClicked(const QUrl& link);
    void completeCommand(const QString& command);
    void onTopicEdited(const QString& topic);
    void onConnectionStatusChanged();
    void onSocketError();
    void applySettings();

private:
    struct Private : public Ui::MessageView {
        ViewInfo::Type viewType;
        QString receiver;
        QPointer<IrcConnection> connection;
        SyntaxHighlighter* highlighter;
        QString topic;
        int sentId;
        QString awayMessage;
        QElapsedTimer awayReply;
        bool playback;
        int joined, parted;
        int connected, disconnected;
        CommandParser* parser;
        QPointer<IrcBuffer> buffer;
        bool stripNicks, showJoins, showParts, showQuits;
        QString timeStampFormat;
        bool firstNames;
    } d;
};

#endif // MESSAGEVIEW_H
