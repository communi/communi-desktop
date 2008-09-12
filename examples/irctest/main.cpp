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

class IrcTest : public QObject
{
    Q_OBJECT

public:
    IrcTest(IrcSession* session)
    {
        session->setParent(this);
        session->setObjectName("irc");
        QMetaObject::connectSlotsByName(this);
    }

protected slots:
    void on_irc_connected();
    void on_irc_nickChanged(const QString& origin, const QString& nick);
    void on_irc_quit(const QString& origin, const QString& message);
    void on_irc_joined(const QString& origin, const QString& channel);
    void on_irc_parted(const QString& origin, const QString& channel, const QString& message);
    void on_irc_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args);
    void on_irc_userModeChanged(const QString& origin, const QString& mode);
    void on_irc_topicChanged(const QString& origin, const QString& channel, const QString& topic);
    void on_irc_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message);
    void on_irc_channelMessageReceived(const QString& origin, const QString& channel, const QString& message);
    void on_irc_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_irc_noticeReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_irc_invited(const QString& origin, const QString& nick, const QString& channel);
    void on_irc_ctcpRequestReceived(const QString& origin, const QString& message);
    void on_irc_ctcpReplyReceived(const QString& origin, const QString& message);
    void on_irc_ctcpActionReceived(const QString& origin, const QString& message);
    void on_irc_unknownMessageReceived(const QString& origin, const QStringList& params);
    void on_irc_numericMessageReceived(const QString& origin, const QStringList& params);
};

void IrcTest::on_irc_connected()
{
    qDebug() << "connected:";
}

void IrcTest::on_irc_nickChanged(const QString& origin, const QString& nick)
{
    qDebug() << "nick:" << origin << nick;
}

void IrcTest::on_irc_quit(const QString& origin, const QString& message)
{
    qDebug() << "quit:" << origin << message;
}

void IrcTest::on_irc_joined(const QString& origin, const QString& channel)
{
    qDebug() << "join:" << origin << channel;
}

void IrcTest::on_irc_parted(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "part:" << origin << channel << message;
}

void IrcTest::on_irc_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args)
{
    qDebug() << "channel_mode:" << origin << channel << mode << args;
}

void IrcTest::on_irc_userModeChanged(const QString& origin, const QString& mode)
{
    qDebug() << "user_mode:" << origin << mode;
}

void IrcTest::on_irc_topicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    qDebug() << "topic:" << origin << channel << topic;
}

void IrcTest::on_irc_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    qDebug() << "kick:" << origin << channel << nick << message;
}

void IrcTest::on_irc_channelMessageReceived(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "channel:" << origin << channel << message;
}

void IrcTest::on_irc_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "private:" << origin << receiver << message;
}

void IrcTest::on_irc_noticeReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "notice:" << origin << receiver << message;
}

void IrcTest::on_irc_invited(const QString& origin, const QString& nick, const QString& channel)
{
    qDebug() << "invite:" << origin << nick << channel;
}

void IrcTest::on_irc_ctcpRequestReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_request:" << origin << message;
}

void IrcTest::on_irc_ctcpReplyReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_reply:" << origin << message;
}

void IrcTest::on_irc_ctcpActionReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_action:" << origin << message;
}

void IrcTest::on_irc_unknownMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "unknown:" << origin << params;
}

void IrcTest::on_irc_numericMessageReceived(const QString& origin, const QStringList& params)
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
    IrcTest test(&session);

    QStringList channels;
    for (int i = 3; i < argc; ++i)
    {
        channels.append(argv[i]);
    }
    session.setAutoJoinChannels(channels);

    if (!session.connectTo(argv[1], 6667, "", argv[2], "nobody", "reality"))
    {
        qWarning("Could not connect: %s", qPrintable(session.errorString()));
        return 1;
    }

    return app.exec();
}

#include "main.moc"
