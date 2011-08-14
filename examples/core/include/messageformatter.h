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

    Q_INVOKABLE QString formatMessage(IrcMessage* message) const;

protected:
    QString formatInviteMessage(IrcInviteMessage* message) const;
    QString formatJoinMessage(IrcJoinMessage* message) const;
    QString formatKickMessage(IrcKickMessage* message) const;
    QString formatModeMessage(IrcModeMessage* message) const;
    QString formatNickMessage(IrcNickMessage* message) const;
    QString formatNoticeMessage(IrcNoticeMessage* message) const;
    QString formatNumericMessage(IrcNumericMessage* message) const;
    QString formatPartMessage(IrcPartMessage* message) const;
    QString formatPrivateMessage(IrcPrivateMessage* message) const;
    QString formatQuitMessage(IrcQuitMessage* message) const;
    QString formatTopicMessage(IrcTopicMessage* message) const;
    QString formatUnknownMessage(IrcMessage* message) const;

    QString prettyUser(const QString& user) const;
};

#endif // MESSAGEFORMATTER_H
