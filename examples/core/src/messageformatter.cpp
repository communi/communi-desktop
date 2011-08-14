/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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

#include "messageformatter.h"
#include <ircprefix.h>
#include <ircutil.h>
#include <qhash.h>

static const QStringList NICK_COLORS = QStringList()
    << QLatin1String("#BF0000")
    << QLatin1String("#FF0000")
    << QLatin1String("#BF7900")
    << QLatin1String("#00BC0F")
    << QLatin1String("#00A7BA")
    << QLatin1String("#0F00B7")
    << QLatin1String("#8100B5");

MessageFormatter::MessageFormatter(QObject* parent) : QObject(parent)
{
}

MessageFormatter::~MessageFormatter()
{
}

QString MessageFormatter::formatMessage(IrcMessage* message) const
{
    switch (message->type())
    {
    case IrcMessage::Invite:
        return formatInviteMessage(static_cast<IrcInviteMessage*>(message));
        break;
    case IrcMessage::Join:
        return formatJoinMessage(static_cast<IrcJoinMessage*>(message));
        break;
    case IrcMessage::Kick:
        return formatKickMessage(static_cast<IrcKickMessage*>(message));
        break;
    case IrcMessage::Mode:
        return formatModeMessage(static_cast<IrcModeMessage*>(message));
        break;
    case IrcMessage::Nick:
        return formatNickMessage(static_cast<IrcNickMessage*>(message));
        break;
    case IrcMessage::Notice:
        return formatNoticeMessage(static_cast<IrcNoticeMessage*>(message));
        break;
    case IrcMessage::Numeric:
        return formatNumericMessage(static_cast<IrcNumericMessage*>(message));
        break;
    case IrcMessage::Part:
        return formatPartMessage(static_cast<IrcPartMessage*>(message));
        break;
    case IrcMessage::Private:
        return formatPrivateMessage(static_cast<IrcPrivateMessage*>(message));
        break;
    case IrcMessage::Quit:
        return formatQuitMessage(static_cast<IrcQuitMessage*>(message));
        break;
    case IrcMessage::Topic:
        return formatTopicMessage(static_cast<IrcTopicMessage*>(message));
        break;
    case IrcMessage::Unknown:
        return formatUnknownMessage(static_cast<IrcMessage*>(message));
        break;
    }
}

QString MessageFormatter::formatInviteMessage(IrcInviteMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    return tr("! %1 invited to %3").arg(prefix, message->channel());
}

QString MessageFormatter::formatJoinMessage(IrcJoinMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    return tr("! %1 joined %2").arg(prefix, message->channel());
}

QString MessageFormatter::formatKickMessage(IrcKickMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    const QString user = prettyUser(message->user());
    if (!message->reason().isEmpty())
        return tr("! %1 kicked %2 (%3)").arg(prefix, user, message->reason());
    else
        return tr("! %1 kicked %2").arg(prefix, user);
}

QString MessageFormatter::formatModeMessage(IrcModeMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    return tr("! %1 sets mode %2 %3 %4").arg(prefix, message->mode(), message->mask(), message->argument());
}

QString MessageFormatter::formatNickMessage(IrcNickMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    const QString nick = prettyUser(message->nick());
    return tr("! %1 changed nick to %2").arg(prefix, nick);
}

QString MessageFormatter::formatNoticeMessage(IrcNoticeMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    const QString msg = IrcUtil::messageToHtml(message->message());
    return tr("[%1] %2").arg(prefix, msg);
}

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    return tr("# %1 %2 %3").arg(prefix, message->command(), message->parameters().join(" "));
}

QString MessageFormatter::formatPartMessage(IrcPartMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    if (!message->reason().isEmpty())
        return tr("! %1 parted %2 (%3)").arg(prefix, message->channel(), message->reason());
    else
        return tr("! %1 parted %2").arg(prefix, message->channel());
}

QString MessageFormatter::formatPrivateMessage(IrcPrivateMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    const QString msg = IrcUtil::messageToHtml(message->message());
    if (message->isAction())
        return tr("* %1 %2").arg(prefix, msg);
    else
        return tr("&lt;%1&gt; %2").arg(prefix, msg);
}

QString MessageFormatter::formatQuitMessage(IrcQuitMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    if (!message->reason().isEmpty())
        return tr("! %1 has quit (%2)").arg(prefix, message->reason());
    else
        return tr("! %1 has quit").arg(prefix);
}

QString MessageFormatter::formatTopicMessage(IrcTopicMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    const QString topic = IrcUtil::messageToHtml(message->topic());
    return tr("! %1 sets topic \"%2\" on %3").arg(prefix, topic, message->channel());
}

QString MessageFormatter::formatUnknownMessage(IrcMessage* message) const
{
    const QString prefix = prettyUser(message->prefix());
    return tr("? %1 %2 %3").arg(prefix, message->command(), message->parameters().join(" "));
}

QString MessageFormatter::prettyUser(const QString& user) const
{
    IrcPrefix prefix(user);
    if (prefix.isValid())
    {
        QString nick = prefix.nick();
        QString color = NICK_COLORS.at(qHash(nick) % NICK_COLORS.count());
        return QString("<span style='color:%1'>%2</span>").arg(color).arg(nick);
    }
    return user;
}
