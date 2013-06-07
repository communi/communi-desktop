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

#include "messageformatter.h"
#include <irctextformat.h>
#include <ircsender.h>
#include <irc.h>
#include <QHash>
#include <QTime>
#include <QColor>
#include <QCoreApplication>
#include <QTextBoundaryFinder>

QString MessageFormatter::formatMessage(IrcMessage* message, const Options& options)
{
    QString formatted;
    switch (message->type()) {
        case IrcMessage::Invite:
            formatted = formatInviteMessage(static_cast<IrcInviteMessage*>(message), options);
            break;
        case IrcMessage::Join:
            formatted = formatJoinMessage(static_cast<IrcJoinMessage*>(message), options);
            break;
        case IrcMessage::Kick:
            formatted = formatKickMessage(static_cast<IrcKickMessage*>(message), options);
            break;
        case IrcMessage::Mode:
            formatted = formatModeMessage(static_cast<IrcModeMessage*>(message), options);
            break;
        case IrcMessage::Names:
            formatted = formatNamesMessage(static_cast<IrcNamesMessage*>(message), options);
            break;
        case IrcMessage::Nick:
            formatted = formatNickMessage(static_cast<IrcNickMessage*>(message), options);
            break;
        case IrcMessage::Notice:
            formatted = formatNoticeMessage(static_cast<IrcNoticeMessage*>(message), options);
            break;
        case IrcMessage::Numeric:
            formatted = formatNumericMessage(static_cast<IrcNumericMessage*>(message), options);
            break;
        case IrcMessage::Part:
            formatted = formatPartMessage(static_cast<IrcPartMessage*>(message), options);
            break;
        case IrcMessage::Pong:
            formatted = formatPongMessage(static_cast<IrcPongMessage*>(message), options);
            break;
        case IrcMessage::Private:
            formatted = formatPrivateMessage(static_cast<IrcPrivateMessage*>(message), options);
            break;
        case IrcMessage::Quit:
            formatted = formatQuitMessage(static_cast<IrcQuitMessage*>(message), options);
            break;
        case IrcMessage::Topic:
            formatted = formatTopicMessage(static_cast<IrcTopicMessage*>(message), options);
            break;
        case IrcMessage::Unknown:
            formatted = formatUnknownMessage(static_cast<IrcMessage*>(message), options);
            break;
        default:
            break;
    }
    const_cast<Options&>(options).timeStamp = message->timeStamp();
    return formatLine(formatted, options);
}

QString MessageFormatter::formatLine(const QString& message, const Options& options)
{
    QString formatted = message;
    if (formatted.isEmpty())
        return QString();

    QString cls = "message";
    if (options.highlight) {
        cls = "highlight";
    } else {
        switch (formatted.at(0).unicode()) {
            case '!': cls = "event"; break;
            case '[': cls = "notice"; break;
            case '*': cls = "action"; break;
            case '?': cls = "unknown"; break;
            default: break;
        }
    }

    if (!options.timeStampFormat.isEmpty())
        formatted = QCoreApplication::translate("MessageFormatter", "<span class='timestamp'>%1</span> %3").arg(options.timeStamp.time().toString(options.timeStampFormat), formatted);

    return QCoreApplication::translate("MessageFormatter", "<span class='%1'>%2</span>").arg(cls, formatted);
}

QString MessageFormatter::formatInviteMessage(IrcInviteMessage* message, const Options& options)
{
    Q_UNUSED(options);
    const QString sender = formatSender(message->sender());
    return QCoreApplication::translate("MessageFormatter", "! %1 invited to %3").arg(sender, message->channel());
}

QString MessageFormatter::formatJoinMessage(IrcJoinMessage* message, const Options& options)
{
    const QString sender = formatSender(message->sender(), options.stripNicks);
    if (message->flags() & IrcMessage::Own && options.repeat)
        return QCoreApplication::translate("MessageFormatter", "! %1 rejoined %2").arg(sender, message->channel());
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 joined %2").arg(sender, message->channel());
}

QString MessageFormatter::formatKickMessage(IrcKickMessage* message, const Options& options)
{
    Q_UNUSED(options);
    const QString sender = formatSender(message->sender());
    const QString user = formatUser(message->user());
    if (!message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 kicked %2 (%3)").arg(sender, user, message->reason());
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 kicked %2").arg(sender, user);
}

QString MessageFormatter::formatModeMessage(IrcModeMessage* message, const Options& options)
{
    const QString sender = formatSender(message->sender());
    if (message->isReply())
        return !options.repeat ? QCoreApplication::translate("MessageFormatter", "! %1 mode is %2 %3").arg(message->target(), message->mode(), message->argument()) : QString();
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 sets mode %2 %3").arg(sender, message->mode(), message->argument());
}

QString MessageFormatter::formatNamesMessage(IrcNamesMessage* message, const Options& options)
{
    if (!options.repeat && !message->names().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 has %2 users").arg(message->channel()).arg(message->names().count());
    return QString();
}

QString MessageFormatter::formatNickMessage(IrcNickMessage* message, const Options& options)
{
    Q_UNUSED(options);
    const QString sender = formatSender(message->sender());
    const QString nick = formatUser(message->nick());
    return QCoreApplication::translate("MessageFormatter", "! %1 changed nick to %2").arg(sender, nick);
}

QString MessageFormatter::formatNoticeMessage(IrcNoticeMessage* message, const Options& options)
{
    if (message->isReply()) {
        const QStringList params = message->message().split(" ", QString::SkipEmptyParts);
        const QString cmd = params.value(0);
        const QString arg = params.value(1);
        if (cmd.toUpper() == "PING")
            return formatPingReply(message->sender(), arg);
        else if (cmd.toUpper() == "TIME")
            return QCoreApplication::translate("MessageFormatter", "! %1 time is %2").arg(formatSender(message->sender()), QStringList(params.mid(1)).join(" "));
        else if (cmd.toUpper() == "VERSION")
            return QCoreApplication::translate("MessageFormatter", "! %1 version is %2").arg(formatSender(message->sender()), QStringList(params.mid(1)).join(" "));
    }

    const QString sender = formatSender(message->sender());
    const QString msg = formatHtml(message->message(), options);
    return QCoreApplication::translate("MessageFormatter", "[%1] %2").arg(sender, msg);
}

#define P_(x) message->parameters().value(x)
#define MID_(x) QStringList(message->parameters().mid(x)).join(" ")

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* message, const Options& options)
{
    if (message->code() < 300)
        return !options.repeat ? QCoreApplication::translate("MessageFormatter", "[INFO] %1").arg(formatHtml(MID_(1), options)) : QString();

    switch (message->code()) {
        case Irc::RPL_MOTDSTART:
        case Irc::RPL_MOTD:
            if (!options.repeat)
                return QCoreApplication::translate("MessageFormatter", "[MOTD] %1").arg(formatHtml(MID_(1), options));
            return QString();
        case Irc::RPL_ENDOFMOTD:
            if (options.repeat)
                return QCoreApplication::translate("MessageFormatter", "! %1 reconnected").arg(options.nickName);
            return QString();
        case Irc::RPL_AWAY:
            return QCoreApplication::translate("MessageFormatter", "! %1 is away (%2)").arg(P_(1), MID_(2));
        case Irc::RPL_ENDOFWHOIS:
            return QString();
        case Irc::RPL_WHOISOPERATOR:
        case Irc::RPL_WHOISMODES: // "is using modes"
        case Irc::RPL_WHOISREGNICK: // "is a registered nick"
        case Irc::RPL_WHOISHELPOP: // "is available for help"
        case Irc::RPL_WHOISSPECIAL: // "is identified to services"
        case Irc::RPL_WHOISHOST: // nick is connecting from <...>
        case Irc::RPL_WHOISSECURE: // nick is using a secure connection
            return QCoreApplication::translate("MessageFormatter", "! %1").arg(MID_(1));
        case Irc::RPL_WHOISUSER:
            return QCoreApplication::translate("MessageFormatter", "! %1 is %2@%3 (%4)").arg(P_(1), P_(2), P_(3), formatHtml(MID_(5), options));
        case Irc::RPL_WHOISSERVER:
            return QCoreApplication::translate("MessageFormatter", "! %1 connected via %2 (%3)").arg(P_(1), P_(2), P_(3));
        case Irc::RPL_WHOISACCOUNT: // nick user is logged in as
            return QCoreApplication::translate("MessageFormatter", "! %1 %3 %2").arg(P_(1), P_(2), P_(3));
        case Irc::RPL_WHOWASUSER:
            return QCoreApplication::translate("MessageFormatter", "! %1 was %2@%3 %4 %5").arg(P_(1), P_(2), P_(3), P_(4), P_(5));
        case Irc::RPL_WHOISIDLE: {
            QDateTime signon = QDateTime::fromTime_t(P_(3).toInt());
            QString idle = formatIdleTime(P_(2).toInt());
            return QCoreApplication::translate("MessageFormatter", "! %1 has been online since %2 (idle for %3)").arg(P_(1), signon.toString(), idle);
        }
        case Irc::RPL_WHOISCHANNELS:
            return QCoreApplication::translate("MessageFormatter", "! %1 is on channels %2").arg(P_(1), P_(2));

        case Irc::RPL_CHANNEL_URL:
            return !options.repeat ? QCoreApplication::translate("MessageFormatter", "! %1 url is %2").arg(P_(1), formatHtml(P_(2), options)) : QString();
        case Irc::RPL_CREATIONTIME:
            if (!options.repeat) {
                QDateTime dateTime = QDateTime::fromTime_t(P_(2).toInt());
                return QCoreApplication::translate("MessageFormatter", "! %1 was created %2").arg(P_(1), dateTime.toString());
            }
            return QString();
        case Irc::RPL_TOPICWHOTIME:
            if (!options.repeat) {
                QDateTime dateTime = QDateTime::fromTime_t(P_(3).toInt());
                return QCoreApplication::translate("MessageFormatter", "! %1 topic was set %2 by %3").arg(P_(1), dateTime.toString(), formatUser(P_(2), options.stripNicks));
            }
            return QString();

        case Irc::RPL_INVITING:
            return QCoreApplication::translate("MessageFormatter", "! inviting %1 to %2").arg(formatUser(P_(1)), P_(2));
        case Irc::RPL_VERSION:
            return QCoreApplication::translate("MessageFormatter", "! %1 version is %2").arg(formatSender(message->sender()), P_(1));
        case Irc::RPL_TIME:
            return QCoreApplication::translate("MessageFormatter", "! %1 time is %2").arg(formatUser(P_(1)), P_(2));
        case Irc::RPL_UNAWAY:
        case Irc::RPL_NOWAWAY:
            return QCoreApplication::translate("MessageFormatter", "! %1").arg(P_(1));

        case Irc::RPL_TOPIC:
        case Irc::RPL_NAMREPLY:
        case Irc::RPL_ENDOFNAMES:
            return QString();

        default:
            if (QByteArray(Irc::toString(message->code())).startsWith("ERR_"))
                return QCoreApplication::translate("MessageFormatter", "[ERROR] %1").arg(formatHtml(MID_(1), options));

            return QCoreApplication::translate("MessageFormatter", "[%1] %2").arg(message->code()).arg(QStringList(message->parameters().mid(1)).join(" "));
    }
}

QString MessageFormatter::formatPartMessage(IrcPartMessage* message, const Options& options)
{
    const QString sender = formatSender(message->sender(), options.stripNicks);
    if (!message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 parted %2 (%3)").arg(sender, message->channel(), formatHtml(message->reason(), options));
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 parted %2").arg(sender, message->channel());
}

QString MessageFormatter::formatPongMessage(IrcPongMessage* message, const Options& options)
{
    Q_UNUSED(options);
    return formatPingReply(message->sender(), message->argument());
}

QString MessageFormatter::formatPrivateMessage(IrcPrivateMessage* message, const Options& options)
{
    const QString sender = formatSender(message->sender());
    const QString msg = formatHtml(message->message(), options);
    if (message->isAction())
        return QCoreApplication::translate("MessageFormatter", "* %1 %2").arg(sender, msg);
    else if (message->isRequest())
        return QCoreApplication::translate("MessageFormatter", "! %1 requested %2").arg(sender, msg.split(" ").value(0).toLower());
    else
        return QCoreApplication::translate("MessageFormatter", "&lt;%1&gt; %2").arg(sender, msg);
}

QString MessageFormatter::formatQuitMessage(IrcQuitMessage* message, const Options& options)
{
    const QString sender = formatSender(message->sender(), options.stripNicks);
    if (!message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 has quit (%2)").arg(sender, formatHtml(message->reason(), options));
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 has quit").arg(sender);
}

QString MessageFormatter::formatTopicMessage(IrcTopicMessage* message, const Options& options)
{
    const QString sender = formatSender(message->sender());
    const QString topic = formatHtml(message->topic(), options);
    const QString channel = message->channel();
    if (message->isReply()) {
        if (topic.isEmpty())
            return !options.repeat ? QCoreApplication::translate("MessageFormatter", "! %1 has no topic set").arg(channel) : QString();
        return !options.repeat ? QCoreApplication::translate("MessageFormatter", "! %1 topic is \"%2\"").arg(channel, topic) : QString();
    }
    return QCoreApplication::translate("MessageFormatter", "! %1 sets topic \"%2\" on %3").arg(sender, topic, channel);
}

QString MessageFormatter::formatUnknownMessage(IrcMessage* message, const Options& options)
{
    Q_UNUSED(options);
    const QString sender = formatSender(message->sender());
    return QCoreApplication::translate("MessageFormatter", "? %1 %2 %3").arg(sender, message->command(), message->parameters().join(" "));
}

QString MessageFormatter::formatPingReply(const IrcSender& sender, const QString& arg)
{
    bool ok;
    int seconds = arg.toInt(&ok);
    if (ok) {
        QDateTime time = QDateTime::fromTime_t(seconds);
        QString result = QString::number(time.secsTo(QDateTime::currentDateTime()));
        return QCoreApplication::translate("MessageFormatter", "! %1 replied in %2s").arg(formatSender(sender), result);
    }
    return QString();
}

QString MessageFormatter::formatSender(const IrcSender& sender, bool strip)
{
    QString name = sender.name();
    if (sender.isValid()) {
        QColor color = QColor::fromHsl(qHash(name) % 359, 255, 64);
        name = QString("<b><a href='nick:%2' style='text-decoration:none; color:%1'>%2</a></b>").arg(color.name()).arg(name);
        if (!strip && !sender.user().isEmpty() && !sender.host().isEmpty())
            name = QString("%1 (%2@%3)").arg(name, sender.user(), sender.host());
    }
    return name;
}

QString MessageFormatter::formatUser(const QString& user, bool strip)
{
    return formatSender(IrcSender(user), strip);
}

QString MessageFormatter::formatIdleTime(int secs)
{
    QStringList idle;
    if (int days = secs / 86400)
        idle += QCoreApplication::translate("MessageFormatter", "%1 days").arg(days);
    secs %= 86400;
    if (int hours = secs / 3600)
        idle += QCoreApplication::translate("MessageFormatter", "%1 hours").arg(hours);
    secs %= 3600;
    if (int mins = secs / 60)
        idle += QCoreApplication::translate("MessageFormatter", "%1 mins").arg(mins);
    idle += QCoreApplication::translate("MessageFormatter", "%1 secs").arg(secs % 60);
    return idle.join(" ");
}

QString MessageFormatter::formatHtml(const QString& message, const Options& options)
{
    QString msg = options.textFormat.toHtml(message);
    for (int i = options.users.count() - 1; i >= 0; --i) {
        const QString& user = options.users.at(i);
        int pos = 0;
        while ((pos = msg.indexOf(user, pos)) != -1) {
            QTextBoundaryFinder finder(QTextBoundaryFinder::Word, msg);

            finder.setPosition(pos);
            if (!finder.isAtBoundary()) {
                pos += user.length();
                continue;
            }

            finder.setPosition(pos + user.length());
            if (!finder.isAtBoundary()) {
                pos += user.length();
                continue;
            }

            const int anchor = msg.indexOf("</a>", pos + user.length() + 1);
            if (anchor != -1 && anchor <= msg.indexOf('<', pos + user.length() + 1)) {
                pos += user.length();
                continue;
            }

            QString formatted = formatUser(msg.mid(pos, user.length()));
            msg.replace(pos, user.length(), formatted);
            pos += formatted.length();
        }
    }
    return msg;
}
