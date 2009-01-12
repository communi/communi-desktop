/*
 * Copyright (C) 2004 Georgy Yunaev tim@krasnogorsk.ru
 * Copyright (C) 2008-2009 J-P Nurmi jpnurmi@gmail.com
 *
 * This example is free, and not covered by LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially. By using it you may give me some credits in your
 * program, but you don't have to.
 *
 *
 * This example tests most features of libirc. It can join the specific
 * channel, welcoming all the people there, and react on some messages -
 * 'help', 'quit', 'dcc chat', 'dcc send', 'ctcp'. Also it can reply to
 * CTCP requests, receive DCC files and accept DCC chats.
 *
 * Features used:
 * - nickname parsing;
 * - handling 'channel' event to track the messages;
 * - handling dcc and ctcp events;
 * - using internal ctcp rely procedure;
 * - generating channel messages;
 * - handling dcc send and dcc chat events;
 * - initiating dcc send and dcc chat.
 *
 * $Id$
 */

#include <QtCore>
#include "coreircsession.h"

class MyIrcSession : public CoreIrcSession
{
    Q_OBJECT

public:
    MyIrcSession(QObject* parent = 0);

protected slots:
    void on_connected();
    void on_nickChanged(const QString& origin, const QString& nick);
    void on_quit(const QString& origin, const QString& message);
    void on_joined(const QString& origin, const QString& channel);
    void on_parted(const QString& origin, const QString& channel, const QString& message);
    void on_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args);
    void on_userModeChanged(const QString& origin, const QString& mode);
    void on_topicChanged(const QString& origin, const QString& channel, const QString& topic);
    void on_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message);
    void on_channelMessageReceived(const QString& origin, const QString& channel, const QString& message);
    void on_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_noticeReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_invited(const QString& origin, const QString& nick, const QString& channel);
    void on_ctcpRequestReceived(const QString& origin, const QString& message);
    void on_ctcpReplyReceived(const QString& origin, const QString& message);
    void on_ctcpActionReceived(const QString& origin, const QString& message);
    void on_unknownMessageReceived(const QString& origin, const QStringList& params);
    void on_numericMessageReceived(const QString& origin, const QStringList& params);
};

MyIrcSession::MyIrcSession(QObject* parent) : CoreIrcSession(parent)
{
    connectSlotsByName(this);
}

void MyIrcSession::on_connected()
{
    qDebug() << "connected:";
}

void MyIrcSession::on_nickChanged(const QString& origin, const QString& nick)
{
    qDebug() << "nick:" << origin << nick;
}

void MyIrcSession::on_quit(const QString& origin, const QString& message)
{
    qDebug() << "quit:" << origin << message;
}

void MyIrcSession::on_joined(const QString& origin, const QString& channel)
{
    qDebug() << "join:" << origin << channel;
}

void MyIrcSession::on_parted(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "part:" << origin << channel << message;
}

void MyIrcSession::on_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args)
{
    qDebug() << "channel_mode:" << origin << channel << mode << args;
}

void MyIrcSession::on_userModeChanged(const QString& origin, const QString& mode)
{
    qDebug() << "user_mode:" << origin << mode;
}

void MyIrcSession::on_topicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    qDebug() << "topic:" << origin << channel << topic;
}

void MyIrcSession::on_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    qDebug() << "kick:" << origin << channel << nick << message;
}

void MyIrcSession::on_channelMessageReceived(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "channel:" << origin << channel << message;
}

void MyIrcSession::on_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "private:" << origin << receiver << message;
}

void MyIrcSession::on_noticeReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "notice:" << origin << receiver << message;
}

void MyIrcSession::on_invited(const QString& origin, const QString& nick, const QString& channel)
{
    qDebug() << "invite:" << origin << nick << channel;
}

void MyIrcSession::on_ctcpRequestReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_request:" << origin << message;
}

void MyIrcSession::on_ctcpReplyReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_reply:" << origin << message;
}

void MyIrcSession::on_ctcpActionReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_action:" << origin << message;
}

void MyIrcSession::on_unknownMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "unknown:" << origin << params;
}

void MyIrcSession::on_numericMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "numeric:" << origin << params;
}

int main (int argc, char* argv[])
{
    if (argc < 4)
    {
        qDebug("Usage: %s <server> <nick> <channels...>", argv[0]);
        return 1;
    }

    QStringList channels;
    for (int i = 3; i < argc; ++i)
    {
        channels.append(argv[i]);
    }

    MyIrcSession session;
    session.setAutoJoinChannels(channels);
    if (!session.connectToServer(argv[1], 6667, argv[2], "nobody", "reality"))
    {
        qWarning("Could not connect: %s", qPrintable(session.errorString()));
        return 1;
    }

    return session.exec();
}

#include "main.moc"
