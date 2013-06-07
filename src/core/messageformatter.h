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

#ifndef MESSAGEFORMATTER_H
#define MESSAGEFORMATTER_H

#include <IrcMessage>
#include <IrcTextFormat>

class MessageFormatter
{
public:
    struct Options
    {
        Options() : repeat(false), highlight(false), stripNicks(false) { }
        bool repeat;
        bool highlight;
        bool stripNicks;
        QString nickName;
        QStringList users;
        QDateTime timeStamp;
        QString timeStampFormat;
        IrcTextFormat textFormat;
    };

    static QString formatMessage(IrcMessage* message, const Options& options = Options());
    static QString formatLine(const QString& message, const Options& options = Options());
    static QString formatHtml(const QString& message, const Options& options = Options());

protected:
    static QString formatInviteMessage(IrcInviteMessage* message, const Options& options);
    static QString formatJoinMessage(IrcJoinMessage* message, const Options& options);
    static QString formatKickMessage(IrcKickMessage* message, const Options& options);
    static QString formatModeMessage(IrcModeMessage* message, const Options& options);
    static QString formatNamesMessage(IrcNamesMessage* message, const Options& options);
    static QString formatNickMessage(IrcNickMessage* message, const Options& options);
    static QString formatNoticeMessage(IrcNoticeMessage* message, const Options& options);
    static QString formatNumericMessage(IrcNumericMessage* message, const Options& options);
    static QString formatPartMessage(IrcPartMessage* message, const Options& options);
    static QString formatPongMessage(IrcPongMessage* message, const Options& options);
    static QString formatPrivateMessage(IrcPrivateMessage* message, const Options& options);
    static QString formatQuitMessage(IrcQuitMessage* message, const Options& options);
    static QString formatTopicMessage(IrcTopicMessage* message, const Options& options);
    static QString formatUnknownMessage(IrcMessage* message, const Options& options);

    static QString formatPingReply(const IrcSender& sender, const QString& arg);

    static QString formatSender(const IrcSender& sender, bool strip = true);
    static QString formatUser(const QString& user, bool strip = true);

    static QString formatIdleTime(int secs);
};

#endif // MESSAGEFORMATTER_H
