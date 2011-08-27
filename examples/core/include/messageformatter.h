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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#ifndef MESSAGEFORMATTER_H
#define MESSAGEFORMATTER_H

#include <QObject>
#include <IrcMessage>

class MessageFormatter : public QObject
{
    Q_OBJECT

public:
    explicit MessageFormatter(QObject* parent = 0);
    virtual ~MessageFormatter();

    Q_INVOKABLE static QString formatMessage(IrcMessage* message);

protected:
    static QString formatInviteMessage(IrcInviteMessage* message);
    static QString formatJoinMessage(IrcJoinMessage* message);
    static QString formatKickMessage(IrcKickMessage* message);
    static QString formatModeMessage(IrcModeMessage* message);
    static QString formatNickMessage(IrcNickMessage* message);
    static QString formatNoticeMessage(IrcNoticeMessage* message);
    static QString formatNumericMessage(IrcNumericMessage* message);
    static QString formatPartMessage(IrcPartMessage* message);
    static QString formatPrivateMessage(IrcPrivateMessage* message);
    static QString formatQuitMessage(IrcQuitMessage* message);
    static QString formatTopicMessage(IrcTopicMessage* message);
    static QString formatUnknownMessage(IrcMessage* message);

    static QString prettyUser(const QString& user);
};

#endif // MESSAGEFORMATTER_H
