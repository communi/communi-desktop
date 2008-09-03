/*
 * Copyright (C) 2004 Georgy Yunaev tim@krasnogorsk.ru
 * Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
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
#include "ircsession.h"
#include "irchandler.h"

class TestIrcHandler : public IrcHandler
{
protected:
    virtual void connected();
    virtual void nickChanged(const QString& origin, const QString& nick);
    virtual void quit(const QString& origin, const QString& message);
    virtual void joined(const QString& origin, const QString& channel);
    virtual void parted(const QString& origin, const QString& channel, const QString& message);
    virtual void channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args);
    virtual void userModeChanged(const QString& origin, const QString& mode);
    virtual void topicChanged(const QString& origin, const QString& channel, const QString& topic);
    virtual void kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message);
    virtual void channelMessageReceived(const QString& origin, const QString& channel, const QString& message);
    virtual void privateMessageReceived(const QString& origin, const QString& receiver, const QString& message);
    virtual void noticeReceived(const QString& origin, const QString& receiver, const QString& message);
    virtual void invited(const QString& origin, const QString& nick, const QString& channel);
    virtual void ctcpRequestReceived(const QString& origin, const QString& message);
    virtual void ctcpReplyReceived(const QString& origin, const QString& message);
    virtual void ctcpActionReceived(const QString& origin, const QString& message);
    virtual void unknownMessageReceived(const QString& origin, const QStringList& params);
    virtual void numericMessageReceived(const QString& origin, const QStringList& params);
};

void TestIrcHandler::connected()
{
    qDebug() << "connected:";
}

void TestIrcHandler::nickChanged(const QString& origin, const QString& nick)
{
    qDebug() << "nick:" << origin << nick;
}

void TestIrcHandler::quit(const QString& origin, const QString& message)
{
    qDebug() << "quit:" << origin << message;
}

void TestIrcHandler::joined(const QString& origin, const QString& channel)
{
    qDebug() << "join:" << origin << channel;
}

void TestIrcHandler::parted(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "part:" << origin << channel << message;
}

void TestIrcHandler::channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args)
{
    qDebug() << "channel_mode:" << origin << channel << mode << args;
}

void TestIrcHandler::userModeChanged(const QString& origin, const QString& mode)
{
    qDebug() << "user_mode:" << origin << mode;
}

void TestIrcHandler::topicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    qDebug() << "topic:" << origin << channel << topic;
}

void TestIrcHandler::kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    qDebug() << "kick:" << origin << channel << nick << message;
}

void TestIrcHandler::channelMessageReceived(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "channel:" << origin << channel << message;
}

void TestIrcHandler::privateMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "private:" << origin << receiver << message;
}

void TestIrcHandler::noticeReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "notice:" << origin << receiver << message;
}

void TestIrcHandler::invited(const QString& origin, const QString& nick, const QString& channel)
{
    qDebug() << "invite:" << origin << nick << channel;
}

void TestIrcHandler::ctcpRequestReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_request:" << origin << message;
}

void TestIrcHandler::ctcpReplyReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_reply:" << origin << message;
}

void TestIrcHandler::ctcpActionReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_action:" << origin << message;
}

void TestIrcHandler::unknownMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "unknown:" << origin << params;
}

void TestIrcHandler::numericMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "numeric:" << origin << params;
}

int main (int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    if (argc < 4)
    {
        qDebug("Usage: %s <server> <nick> <channels...>", argv[0]);
        return 1;
    }

    IrcSession session;

    TestIrcHandler handler;
    session.addHandler(&handler);

    QStringList channels;
    for (int i = 3; i < argc; ++i)
    {
        channels.append(argv[i]);
    }
    session.setAutoJoinChannels(channels);

    if (!session.connectToServer(argv[1], 6667, "", argv[2], "nobody", "reality"))
    {
        qWarning("Could not connect: %s", qPrintable(session.errorString()));
        return 1;
    }

    session.exec();
    return 0;
}
