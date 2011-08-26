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
#include <QHash>
#include <QTime>

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
    QString formatted;
    switch (message->type())
    {
    case IrcMessage::Invite:
        formatted = formatInviteMessage(static_cast<IrcInviteMessage*>(message));
        break;
    case IrcMessage::Join:
        formatted = formatJoinMessage(static_cast<IrcJoinMessage*>(message));
        break;
    case IrcMessage::Kick:
        formatted = formatKickMessage(static_cast<IrcKickMessage*>(message));
        break;
    case IrcMessage::Mode:
        formatted = formatModeMessage(static_cast<IrcModeMessage*>(message));
        break;
    case IrcMessage::Nick:
        formatted = formatNickMessage(static_cast<IrcNickMessage*>(message));
        break;
    case IrcMessage::Notice:
        formatted = formatNoticeMessage(static_cast<IrcNoticeMessage*>(message));
        break;
    case IrcMessage::Numeric:
        formatted = formatNumericMessage(static_cast<IrcNumericMessage*>(message));
        break;
    case IrcMessage::Part:
        formatted = formatPartMessage(static_cast<IrcPartMessage*>(message));
        break;
    case IrcMessage::Private:
        formatted = formatPrivateMessage(static_cast<IrcPrivateMessage*>(message));
        break;
    case IrcMessage::Quit:
        formatted = formatQuitMessage(static_cast<IrcQuitMessage*>(message));
        break;
    case IrcMessage::Topic:
        formatted = formatTopicMessage(static_cast<IrcTopicMessage*>(message));
        break;
    case IrcMessage::Unknown:
        formatted = formatUnknownMessage(static_cast<IrcMessage*>(message));
        break;
    }
    const QString time = QTime::currentTime().toString();
    return tr("[%1] %2").arg(time).arg(formatted);
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

#define P_(x) message->parameters().value(x)

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* message) const
{
    if (message->code() < 300)
        return tr("[INFO] %1").arg(QStringList(message->parameters().mid(1)).join(" "));
    if (message->code() > 399)
        return tr("[ERROR] %1").arg(QStringList(message->parameters().mid(1)).join(" "));

    switch (message->code())
    {
    case Irc::RPL_MOTDSTART:
    case Irc::RPL_MOTD:
        return tr("[MOTD] %1").arg(QStringList(message->parameters().mid(1)).join(" "));
    case Irc::RPL_AWAY:
    case Irc::RPL_WHOISOPERATOR:
    case 310: // "is available for help"
    case 320: // "is identified to services"
    case 378: // nick is connecting from <...>
    case 671: // nick is using a secure connection
        return tr("! %1 %2").arg(message->prefix(), message->parameters().join(" "));
    case Irc::RPL_WHOISUSER:
        return tr("! %1 is %2@%3 %4").arg(P_(1), P_(2), P_(3), P_(4));
    case Irc::RPL_WHOISSERVER:
        return tr("! %1 online via %2 (%3)").arg(P_(1), P_(2), P_(3));
    case 330: // nick user is logged in as
        return tr("! %1 %3 %2").arg(P_(1), P_(2), P_(3));
    case Irc::RPL_WHOWASUSER:
        return tr("! %1 was %2@%3 %4 %5").arg(P_(1), P_(2), P_(3), P_(4), P_(5));
    case Irc::RPL_WHOISIDLE: {
        QDateTime signon = QDateTime::fromTime_t(P_(3).toInt());
        QTime idle = QTime().addSecs(P_(2).toInt());
        return tr("! %1 has been online since %2 (idle for %3)").arg(P_(1), signon.toString(), idle.toString());
    }
    case Irc::RPL_WHOISCHANNELS:
        return tr("! %1 is on channels %2").arg(P_(1), P_(2));
    case Irc::RPL_CHANNELMODEIS:
        return tr("! %1 mode is %2").arg(P_(1), P_(2));
    case Irc::RPL_CHANNELURL:
        return tr("! %1 url is %2").arg(P_(1), P_(2));
    case Irc::RPL_CHANNELCREATED: {
        QDateTime dateTime = QDateTime::fromTime_t(P_(2).toInt());
        return tr("! %1 was created %2").arg(P_(1), dateTime.toString());
    }
    case Irc::RPL_NOTOPIC:
        return tr("! %1 has no topic set").arg(P_(1));
    case Irc::RPL_TOPIC:
        return tr("! %1 topic is \"%2\"").arg(P_(1), P_(2));
    case Irc::RPL_TOPICSET: {
        QDateTime dateTime = QDateTime::fromTime_t(P_(3).toInt());
        return tr("! %1 topic was set %2 by %3").arg(P_(1), dateTime.toString(), P_(2));
    }
    case Irc::RPL_INVITING:
        return tr("! %1 is inviting %1 to %2").arg(message->prefix(), P_(1), P_(2));
    case Irc::RPL_VERSION:
        return tr("! %1 version is %2").arg(message->prefix(), P_(1));
    case Irc::RPL_TIME:
        return tr("! %1 time is %2").arg(P_(1), P_(2));

    default:
        return tr("[%1] %2").arg(message->code()).arg(QStringList(message->parameters().mid(1)).join(" "));
    }
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
