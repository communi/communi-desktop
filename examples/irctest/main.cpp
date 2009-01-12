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
#include <IrcSession>

class MyIrcSession : public Irc::Session
{
    Q_OBJECT

public:
    MyIrcSession(QObject* parent = 0);

protected slots:
    void on_connected();
    void on_disconnected();

    void on_msgJoined(const QString& origin, const QString& channel);
    void on_msgParted(const QString& origin, const QString& channel, const QString& message);
    void on_msgQuit(const QString& origin, const QString& message);
    void on_msgNickChanged(const QString& origin, const QString& nick);
    void on_msgModeChanged(const QString& origin, const QString& receiver, const QString& mode, const QString& args);
    void on_msgTopicChanged(const QString& origin, const QString& channel, const QString& topic);
    void on_msgInvited(const QString& origin, const QString& receiver, const QString& channel);
    void on_msgKicked(const QString& origin, const QString& channel, const QString& nick, const QString& message);
    void on_msgMessageReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_msgNoticeReceived(const QString& origin, const QString& receiver, const QString& notice);
    void on_msgCtcpRequestReceived(const QString& origin, const QString& request);
    void on_msgCtcpReplyReceived(const QString& origin, const QString& reply);
    void on_msgCtcpActionReceived(const QString& origin, const QString& receiver, const QString& action);
    void on_msgNumericMessageReceived(const QString& origin, uint code, const QStringList& params);
    void on_msgUnknownMessageReceived(const QString& origin, const QStringList& params);
};

MyIrcSession::MyIrcSession(QObject* parent) : Irc::Session(parent)
{
    connectSlotsByName(this);
}

void MyIrcSession::on_connected()
{
    qDebug() << "connected:";
}

void MyIrcSession::on_disconnected()
{
    qDebug() << "disconnected:";
}

void MyIrcSession::on_msgJoined(const QString& origin, const QString& channel)
{
    qDebug() << "join:" << origin << channel;
}

void MyIrcSession::on_msgParted(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "part:" << origin << channel << message;
}

void MyIrcSession::on_msgQuit(const QString& origin, const QString& message)
{
    qDebug() << "quit:" << origin << message;
}

void MyIrcSession::on_msgNickChanged(const QString& origin, const QString& nick)
{
    qDebug() << "nick:" << origin << nick;
}

void MyIrcSession::on_msgModeChanged(const QString& origin, const QString& receiver, const QString& mode, const QString& args)
{
    qDebug() << "mode:" << origin << receiver << mode << args;
}

void MyIrcSession::on_msgTopicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    qDebug() << "topic:" << origin << channel << topic;
}

void MyIrcSession::on_msgInvited(const QString& origin, const QString& receiver, const QString& channel)
{
    qDebug() << "invite:" << origin << receiver << channel;
}

void MyIrcSession::on_msgKicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    qDebug() << "kick:" << origin << channel << nick << message;
}

void MyIrcSession::on_msgMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "message:" << origin << receiver << message;
}

void MyIrcSession::on_msgNoticeReceived(const QString& origin, const QString& receiver, const QString& notice)
{
    qDebug() << "notice:" << origin << receiver << notice;
}

void MyIrcSession::on_msgCtcpRequestReceived(const QString& origin, const QString& request)
{
    qDebug() << "ctcp request:" << origin << request;
}

void MyIrcSession::on_msgCtcpReplyReceived(const QString& origin, const QString& reply)
{
    qDebug() << "ctcp reply:" << origin << reply;
}

void MyIrcSession::on_msgCtcpActionReceived(const QString& origin, const QString& receiver, const QString& action)
{
    qDebug() << "ctcp action:" << origin << receiver << action;
}

void MyIrcSession::on_msgNumericMessageReceived(const QString& origin, uint code, const QStringList& params)
{
    qDebug() << "numeric:" << origin << code << params;
}

void MyIrcSession::on_msgUnknownMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "unknown:" << origin << params;
}

int main (int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
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
    session.setNick(argv[2]);
    session.setAutoJoinChannels(channels);
    session.connectToServer(argv[1], 6667);

    return app.exec();
}

#include "main.moc"
