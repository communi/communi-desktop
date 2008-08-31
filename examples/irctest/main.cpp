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

#include "ircsession.h"

#if 0
void event_join (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    dump_event (session, event, origin, params, count);
    irc_cmd_user_mode (session, "+i");
    irc_cmd_msg (session, params[0], "Hi all");
}


void event_connect (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    irc_ctx_t * ctx = (irc_ctx_t *) irc_get_ctx (session);
    dump_event (session, event, origin, params, count);

    irc_cmd_join (session, ctx->channel, 0);
}


void event_privmsg (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    dump_event (session, event, origin, params, count);

    printf ("'%s' said me (%s): %s\n",
        origin ? origin : "someone",
        params[0], params[1] );
}


void dcc_recv_callback (irc_session_t * session, irc_dcc_t id, int status, void * ctx, const char * data, unsigned int length)
{
    static int count = 1;
    char buf[12];

    switch (status)
    {
    case LIBIRC_ERR_CLOSED:
        printf ("DCC %d: chat closed\n", id);
        break;

    case 0:
        if ( !data )
        {
            printf ("DCC %d: chat connected\n", id);
            irc_dcc_msg (session, id, "Hehe");
        }
        else
        {
            printf ("DCC %d: %s\n", id, data);
            sprintf (buf, "DCC [%d]: %d", id, count++);
            irc_dcc_msg (session, id, buf);
        }
        break;

    default:
        printf ("DCC %d: error %s\n", id, irc_strerror(status));
        break;
    }
}


void dcc_file_recv_callback (irc_session_t * session, irc_dcc_t id, int status, void * ctx, const char * data, unsigned int length)
{
    if ( status == 0 && length == 0 )
    {
        printf ("File sent successfully\n");

        if ( ctx )
            fclose ((FILE*) ctx);
    }
    else if ( status )
    {
        printf ("File sent error: %d\n", status);

        if ( ctx )
            fclose ((FILE*) ctx);
    }
    else
    {
        if ( ctx )
            fwrite (data, 1, length, (FILE*) ctx);
        printf ("File sent progress: %d\n", length);
    }
}


void event_channel (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    char nickbuf[128];

    if ( count != 2 )
        return;

    printf ("'%s' said in channel %s: %s\n",
        origin ? origin : "someone",
        params[0], params[1] );

    if ( !origin )
        return;

    irc_target_get_nick (origin, nickbuf, sizeof(nickbuf));

    if ( !strcmp (params[1], "quit") )
        irc_cmd_quit (session, "of course, Master!");

    if ( !strcmp (params[1], "help") )
    {
        irc_cmd_msg (session, params[0], "quit, help, dcc chat, dcc send, ctcp");
    }

    if ( !strcmp (params[1], "ctcp") )
    {
        irc_cmd_ctcp_request (session, nickbuf, "PING 223");
        irc_cmd_ctcp_request (session, nickbuf, "FINGER");
        irc_cmd_ctcp_request (session, nickbuf, "VERSION");
        irc_cmd_ctcp_request (session, nickbuf, "TIME");
    }

    if ( !strcmp (params[1], "dcc chat") )
    {
        irc_dcc_t dccid;
        irc_dcc_chat (session, 0, nickbuf, dcc_recv_callback, &dccid);
        printf ("DCC chat ID: %d\n", dccid);
    }

    if ( !strcmp (params[1], "dcc send") )
    {
        irc_dcc_t dccid;
        irc_dcc_sendfile (session, 0, nickbuf, "irctest.c", dcc_file_recv_callback, &dccid);
        printf ("DCC send ID: %d\n", dccid);
    }

    if ( !strcmp (params[1], "topic") )
        irc_cmd_topic (session, params[0], 0);
    else if ( strstr (params[1], "topic ") == params[1] )
        irc_cmd_topic (session, params[0], params[1] + 6);

    if ( strstr (params[1], "mode ") == params[1] )
        irc_cmd_channel_mode (session, params[0], params[1] + 5);

    if ( strstr (params[1], "nick ") == params[1] )
        irc_cmd_nick (session, params[1] + 5);

    if ( strstr (params[1], "whois ") == params[1] )
        irc_cmd_whois (session, params[1] + 5);
}


void irc_event_dcc_chat (irc_session_t * session, const char * nick, const char * addr, irc_dcc_t dccid)
{
    printf ("DCC chat [%d] requested from '%s' (%s)\n", dccid, nick, addr);

    irc_dcc_accept (session, dccid, 0, dcc_recv_callback);
}


void irc_event_dcc_send (irc_session_t * session, const char * nick, const char * addr, const char * filename, unsigned long size, irc_dcc_t dccid)
{
    FILE * fp;
    printf ("DCC send [%d] requested from '%s' (%s): %s (%lu bytes)\n", dccid, nick, addr, filename, size);

    if ( (fp = fopen ("file", "wb")) == 0 )
        abort();

    irc_dcc_accept (session, dccid, fp, dcc_file_recv_callback);
}

void event_numeric (irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
    char buf[24];
    sprintf (buf, "%d", event);

    dump_event (session, buf, origin, params, count);
}

#endif

int main (int argc, char **argv)
{
    //QCoreApplication app(argc, argv);

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

    IrcSession session;
    session.setAutoJoinChannels(channels);

    if (!session.connectToServer(argv[1], 6667, "", argv[2], "", ""))
    {
        qWarning("Could not connect: %s", qPrintable(session.errorString()));
        return 1;
    }

    session.run();
    return 0;
}
