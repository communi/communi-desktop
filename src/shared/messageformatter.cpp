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
#include <ircpalette.h>
#include <ircsender.h>
#include <irc.h>
#include <QHash>
#include <QTime>
#include <QColor>
#include <QTextBoundaryFinder>

static IrcTextFormat IRC_TEXT_FORMAT;

MessageFormatter::MessageFormatter(QObject* parent) : QObject(parent)
{
    d.highlight = false;
    d.timeStamp = false;
    d.stripNicks = true;
    d.zncPlayback = false;
    d.messageType = IrcMessage::Unknown;

    static bool init = false;
    if (!init) {
        init = true;
        IrcPalette palette;
        QStringList colorNames = QStringList()
                << "navy" << "green" << "red" << "maroon" << "purple" << "olive"
                << "yellow" << "lime" << "teal" << "aqua" << "royalblue" << "fuchsia";
        for (int i = IrcPalette::Blue; i <= IrcPalette::Pink; ++i) {
            QColor color(colorNames.takeFirst());
            color.setHsl(color.hue(), 100, 82);
            palette.setColorName(i, color.name());
        }
        IRC_TEXT_FORMAT.setPalette(palette);
    }
}

MessageFormatter::~MessageFormatter()
{
}

bool MessageFormatter::hasHighlight() const
{
    return d.highlight;
}

QStringList MessageFormatter::highlights() const
{
    return d.highlights;
}

void MessageFormatter::setHighlights(const QStringList& highlights)
{
    d.highlights = highlights;
}

QStringList MessageFormatter::users() const
{
    return d.users;
}

void MessageFormatter::setUsers(const QStringList& users)
{
    d.users = users;
}

bool MessageFormatter::timeStamp() const
{
    return d.timeStamp;
}

void MessageFormatter::setTimeStamp(bool timeStamp)
{
    d.timeStamp = timeStamp;
}

bool MessageFormatter::stripNicks() const
{
    return d.stripNicks;
}

void MessageFormatter::setStripNicks(bool strip)
{
    d.stripNicks = strip;
}

bool MessageFormatter::zncPlaybackMode() const
{
    return d.zncPlayback;
}

void MessageFormatter::setZncPlaybackMode(bool enabled)
{
    d.zncPlayback = enabled;
}

IrcMessage::Type MessageFormatter::effectiveMessageType() const
{
    return d.messageType;
}

QString MessageFormatter::timeStampFormat() const
{
    return d.timeStampFormat;
}

void MessageFormatter::setTimeStampFormat(const QString& format)
{
    d.timeStampFormat = format;
}

QString MessageFormatter::formatMessage(IrcMessage* message) const
{
    QString formatted;
    d.highlight = false;
    d.messageType = IrcMessage::Unknown;
    switch (message->type()) {
        case IrcMessage::Invite:
            formatted = formatInviteMessage(static_cast<IrcInviteMessage*>(message));
            break;
        case IrcMessage::Join:
            if (message->flags() & IrcMessage::Own)
                d.receivedCodes.clear();
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
            if (d.zncPlayback)
                formatted = formatZncPlaybackMessage(static_cast<IrcNoticeMessage*>(message));
            else
                formatted = formatNoticeMessage(static_cast<IrcNoticeMessage*>(message));
            break;
        case IrcMessage::Numeric:
            formatted = formatNumericMessage(static_cast<IrcNumericMessage*>(message));
            break;
        case IrcMessage::Part:
            formatted = formatPartMessage(static_cast<IrcPartMessage*>(message));
            break;
        case IrcMessage::Pong:
            formatted = formatPongMessage(static_cast<IrcPongMessage*>(message));
            break;
        case IrcMessage::Private:
            if (d.zncPlayback)
                formatted = formatZncPlaybackMessage(static_cast<IrcPrivateMessage*>(message));
            else
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
        default:
            break;
    }

    return formatMessage(message->timeStamp(), formatted);
}

QString MessageFormatter::formatMessage(const QDateTime& timeStamp, const QString& message) const
{
    QString formatted = message;
    if (formatted.isEmpty())
        return QString();

    QString cls = "message";
    if (d.highlight) {
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

    if (d.timeStamp)
        formatted = tr("<span class='timestamp'>%1</span> %3").arg(timeStamp.time().toString(d.timeStampFormat), formatted);

    return tr("<span class='%1'>%2</span>").arg(cls, formatted);
}

QString MessageFormatter::formatInviteMessage(IrcInviteMessage* message) const
{
    d.messageType = IrcMessage::Invite;
    const QString sender = formatSender(message->sender());
    return tr("! %1 invited to %3").arg(sender, message->channel());
}

QString MessageFormatter::formatJoinMessage(IrcJoinMessage* message) const
{
    d.messageType = IrcMessage::Join;
    const QString sender = formatSender(message->sender(), d.stripNicks);
    return tr("! %1 joined %2").arg(sender, message->channel());
}

QString MessageFormatter::formatKickMessage(IrcKickMessage* message) const
{
    d.messageType = IrcMessage::Kick;
    const QString sender = formatSender(message->sender());
    const QString user = formatUser(message->user());
    if (!message->reason().isEmpty())
        return tr("! %1 kicked %2 (%3)").arg(sender, user, message->reason());
    else
        return tr("! %1 kicked %2").arg(sender, user);
}

QString MessageFormatter::formatModeMessage(IrcModeMessage* message) const
{
    d.messageType = IrcMessage::Mode;
    const QString sender = formatSender(message->sender());
    return tr("! %1 sets mode %2 %3").arg(sender, message->mode(), message->argument());
}

QString MessageFormatter::formatNickMessage(IrcNickMessage* message) const
{
    d.messageType = IrcMessage::Nick;
    const QString sender = formatSender(message->sender());
    const QString nick = formatUser(message->nick());
    return tr("! %1 changed nick to %2").arg(sender, nick);
}

QString MessageFormatter::formatNoticeMessage(IrcNoticeMessage* message) const
{
    d.messageType = IrcMessage::Notice;
    if (message->isReply()) {
        const QStringList params = message->message().split(" ", QString::SkipEmptyParts);
        const QString cmd = params.value(0);
        const QString arg = params.value(1);
        if (cmd.toUpper() == "PING")
            return formatPingReply(message->sender(), arg);
        else if (cmd.toUpper() == "TIME")
            return tr("! %1 time is %2").arg(formatSender(message->sender()), QStringList(params.mid(1)).join(" "));
        else if (cmd.toUpper() == "VERSION")
            return tr("! %1 version is %2").arg(formatSender(message->sender()), QStringList(params.mid(1)).join(" "));
    }

    foreach (const QString& hilite, d.highlights) {
        if (message->message().contains(hilite))
            d.highlight = true;
    }
    const QString sender = formatSender(message->sender());
    const QString msg = formatHtml(message->message());
    return tr("[%1] %2").arg(sender, msg);
}

#define P_(x) message->parameters().value(x)
#define MID_(x) QStringList(message->parameters().mid(x)).join(" ")

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* message) const
{
    d.messageType = IrcMessage::Numeric;
    if (message->code() == Irc::RPL_WELCOME)
        d.receivedCodes.clear();
    d.receivedCodes += message->code();

    if (message->code() < 300)
        return tr("[INFO] %1").arg(formatHtml(MID_(1)));

    switch (message->code()) {
        case Irc::RPL_MOTDSTART:
        case Irc::RPL_MOTD:
            return tr("[MOTD] %1").arg(formatHtml(MID_(1)));
        case Irc::RPL_ENDOFMOTD:
            return QString();
        case Irc::RPL_AWAY:
            return tr("! %1 is away (%2)").arg(P_(1), MID_(2));
        case Irc::RPL_ENDOFWHOIS:
            return QString();
        case Irc::RPL_WHOISOPERATOR:
        case Irc::RPL_WHOISMODES: // "is using modes"
        case Irc::RPL_WHOISREGNICK: // "is a registered nick"
        case Irc::RPL_WHOISHELPOP: // "is available for help"
        case Irc::RPL_WHOISSPECIAL: // "is identified to services"
        case Irc::RPL_WHOISHOST: // nick is connecting from <...>
        case Irc::RPL_WHOISSECURE: // nick is using a secure connection
            return tr("! %1").arg(MID_(1));
        case Irc::RPL_WHOISUSER:
            return tr("! %1 is %2@%3 (%4)").arg(P_(1), P_(2), P_(3), formatHtml(MID_(5)));
        case Irc::RPL_WHOISSERVER:
            return tr("! %1 connected via %2 (%3)").arg(P_(1), P_(2), P_(3));
        case Irc::RPL_WHOISACCOUNT: // nick user is logged in as
            return tr("! %1 %3 %2").arg(P_(1), P_(2), P_(3));
        case Irc::RPL_WHOWASUSER:
            return tr("! %1 was %2@%3 %4 %5").arg(P_(1), P_(2), P_(3), P_(4), P_(5));
        case Irc::RPL_WHOISIDLE: {
            QDateTime signon = QDateTime::fromTime_t(P_(3).toInt());
            QString idle = formatIdleTime(P_(2).toInt());
            return tr("! %1 has been online since %2 (idle for %3)").arg(P_(1), signon.toString(), idle);
        }
        case Irc::RPL_WHOISCHANNELS:
            return tr("! %1 is on channels %2").arg(P_(1), P_(2));
        case Irc::RPL_CHANNELMODEIS:
            return tr("! %1 mode is %2").arg(P_(1), P_(2));
        case Irc::RPL_CHANNEL_URL:
            return tr("! %1 url is %2").arg(P_(1), formatHtml(P_(2)));
        case Irc::RPL_CREATIONTIME: {
            QDateTime dateTime = QDateTime::fromTime_t(P_(2).toInt());
            return tr("! %1 was created %2").arg(P_(1), dateTime.toString());
        }
        case Irc::RPL_NOTOPIC:
            return tr("! %1 has no topic set").arg(P_(1));
        case Irc::RPL_TOPIC:
            return tr("! %1 topic is \"%2\"").arg(P_(1), formatHtml(P_(2)));
        case Irc::RPL_TOPICWHOTIME: {
            QDateTime dateTime = QDateTime::fromTime_t(P_(3).toInt());
            return tr("! %1 topic was set %2 by %3").arg(P_(1), dateTime.toString(), formatUser(P_(2), d.stripNicks));
        }
        case Irc::RPL_INVITING:
            return tr("! inviting %1 to %2").arg(formatUser(P_(1)), P_(2));
        case Irc::RPL_VERSION:
            return tr("! %1 version is %2").arg(formatSender(message->sender()), P_(1));
        case Irc::RPL_TIME:
            return tr("! %1 time is %2").arg(formatUser(P_(1)), P_(2));
        case Irc::RPL_UNAWAY:
        case Irc::RPL_NOWAWAY:
            return tr("! %1").arg(P_(1));

        case Irc::RPL_NAMREPLY:
            if (d.receivedCodes.contains(Irc::RPL_ENDOFNAMES)) {
                int count = message->parameters().count();
                QString channel = message->parameters().value(count - 2);
                QStringList names = message->parameters().value(count - 1).split(" ", QString::SkipEmptyParts);
                return tr("! %1 users: %2").arg(channel).arg(names.join(" "));
            }
            return QString();

        case Irc::RPL_ENDOFNAMES:
            if (!d.users.isEmpty() && !d.receivedCodes.mid(0, d.receivedCodes.count() - 1).contains(Irc::RPL_ENDOFNAMES))
                return tr("! %1 has %2 users").arg(message->parameters().value(1)).arg(d.users.count());
            return QString();

        default:
            if (QByteArray(Irc::toString(message->code())).startsWith("ERR_"))
                return tr("[ERROR] %1").arg(formatHtml(MID_(1)));

            return tr("[%1] %2").arg(message->code()).arg(QStringList(message->parameters().mid(1)).join(" "));
    }
}

QString MessageFormatter::formatPartMessage(IrcPartMessage* message) const
{
    d.messageType = IrcMessage::Part;
    const QString sender = formatSender(message->sender(), d.stripNicks);
    if (!message->reason().isEmpty())
        return tr("! %1 parted %2 (%3)").arg(sender, message->channel(), formatHtml(message->reason()));
    else
        return tr("! %1 parted %2").arg(sender, message->channel());
}

QString MessageFormatter::formatPongMessage(IrcPongMessage* message) const
{
    d.messageType = IrcMessage::Pong;
    return formatPingReply(message->sender(), message->argument());
}

QString MessageFormatter::formatPrivateMessage(IrcPrivateMessage* message) const
{
    d.messageType = IrcMessage::Private;
    foreach (const QString& hilite, d.highlights) {
        if (message->message().contains(hilite))
            d.highlight = true;
    }
    const QString sender = formatSender(message->sender());
    const QString msg = formatHtml(message->message());
    if (message->isAction())
        return tr("* %1 %2").arg(sender, msg);
    else if (message->isRequest())
        return tr("! %1 requested %2").arg(sender, msg.split(" ").value(0).toLower());
    else
        return tr("&lt;%1&gt; %2").arg(sender, msg);
}

QString MessageFormatter::formatQuitMessage(IrcQuitMessage* message) const
{
    d.messageType = IrcMessage::Quit;
    const QString sender = formatSender(message->sender(), d.stripNicks);
    if (!message->reason().isEmpty())
        return tr("! %1 has quit (%2)").arg(sender, formatHtml(message->reason()));
    else
        return tr("! %1 has quit").arg(sender);
}

QString MessageFormatter::formatTopicMessage(IrcTopicMessage* message) const
{
    d.messageType = IrcMessage::Topic;
    const QString sender = formatSender(message->sender());
    const QString topic = formatHtml(message->topic());
    return tr("! %1 sets topic \"%2\" on %3").arg(sender, topic, message->channel());
}

QString MessageFormatter::formatUnknownMessage(IrcMessage* message) const
{
    d.messageType = IrcMessage::Unknown;
    const QString sender = formatSender(message->sender());
    return tr("? %1 %2 %3").arg(sender, message->command(), message->parameters().join(" "));
}

QString MessageFormatter::formatZncPlaybackMessage(IrcPrivateMessage* message) const
{
    QString msg = message->message();
    int idx = msg.indexOf(" ");
    if (idx != -1) {
        QDateTime timeStamp = QDateTime::fromString(msg.left(idx), d.timeStampFormat);
        if (timeStamp.isValid()) {
            message->setTimeStamp(timeStamp);
            msg.remove(0, idx + 1);

            if (message->sender().name() == "*buffextras") {
                idx = msg.indexOf(" ");
                IrcSender sender(msg.left(idx));
                QString content = msg.mid(idx + 1);

                IrcMessage* tmp = 0;
                if (content.startsWith("joined")) {
                    tmp = IrcMessage::fromParameters(sender.prefix(), "JOIN", QStringList() << message->target(), message->session());
                    content = formatJoinMessage(static_cast<IrcJoinMessage*>(tmp));
                } else if (content.startsWith("parted")) {
                    QString reason = content.mid(content.indexOf("[") + 1);
                    reason.chop(1);
                    tmp = IrcMessage::fromParameters(sender.prefix(), "PART", QStringList() << reason , message->session());
                    content = formatPartMessage(static_cast<IrcPartMessage*>(tmp));
                } else if (content.startsWith("quit")) {
                    QString reason = content.mid(content.indexOf("[") + 1);
                    reason.chop(1);
                    tmp = IrcMessage::fromParameters(sender.prefix(), "QUIT", QStringList() << reason , message->session());
                    content = formatQuitMessage(static_cast<IrcQuitMessage*>(tmp));
                } else if (content.startsWith("is")) {
                    QStringList tokens = content.split(" ", QString::SkipEmptyParts);
                    tmp = IrcMessage::fromParameters(sender.prefix(), "NICK", QStringList() << tokens.last() , message->session());
                    content = formatNickMessage(static_cast<IrcNickMessage*>(tmp));
                } else if (content.startsWith("set")) {
                    QStringList tokens = content.split(" ", QString::SkipEmptyParts);
                    QString user = tokens.takeLast();
                    QString mode = tokens.takeLast();
                    tmp = IrcMessage::fromParameters(sender.prefix(), "MODE", QStringList() << message->target() << mode << user, message->session());
                    content = formatModeMessage(static_cast<IrcModeMessage*>(tmp));
                } else if (content.startsWith("changed")) {
                    QString topic = content.mid(content.indexOf(":") + 2);
                    tmp = IrcMessage::fromParameters(sender.prefix(), "TOPIC", QStringList() << message->target() << topic, message->session());
                    content = formatTopicMessage(static_cast<IrcTopicMessage*>(tmp));
                } else if (content.startsWith("kicked")) {
                    QString reason = content.mid(content.indexOf("[") + 1);
                    reason.chop(1);
                    QStringList tokens = content.split(" ", QString::SkipEmptyParts);
                    tmp = IrcMessage::fromParameters(sender.prefix(), "KICK", QStringList() << message->target() << tokens.value(2) << reason, message->session());
                    content = formatKickMessage(static_cast<IrcKickMessage*>(tmp));
                }
                if (tmp) {
                    delete tmp;
                    return content;
                }
            }

            if (message->isAction())
                msg = QString("\1ACTION %1\1").arg(msg);
            else if (message->isRequest())
                msg = QString("\1%1\1").arg(msg);
            message->setParameters(QStringList() << message->target() << msg);
        }
    }
    return formatPrivateMessage(message);
}

QString MessageFormatter::formatZncPlaybackMessage(IrcNoticeMessage* message) const
{
    QString msg = message->message();
    int idx = msg.indexOf(" ");
    if (idx != -1) {
        QDateTime timeStamp = QDateTime::fromString(msg.left(idx), d.timeStampFormat);
        if (timeStamp.isValid()) {
            message->setTimeStamp(timeStamp);
            msg.remove(0, idx + 1);
            if (message->isReply())
                msg = QString("\1%1\1").arg(msg);
            message->setParameters(QStringList() << message->target() << msg);
        }
    }
    return formatNoticeMessage(message);
}

QString MessageFormatter::formatPingReply(const IrcSender& sender, const QString& arg)
{
    bool ok;
    int seconds = arg.toInt(&ok);
    if (ok) {
        QDateTime time = QDateTime::fromTime_t(seconds);
        QString result = QString::number(time.secsTo(QDateTime::currentDateTime()));
        return tr("! %1 replied in %2s").arg(formatSender(sender), result);
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
        idle += tr("%1 days").arg(days);
    secs %= 86400;
    if (int hours = secs / 3600)
        idle += tr("%1 hours").arg(hours);
    secs %= 3600;
    if (int mins = secs / 60)
        idle += tr("%1 mins").arg(mins);
    idle += tr("%1 secs").arg(secs % 60);
    return idle.join(" ");
}

#if QT_VERSION >= 0x050000
#   define BOUNDARY_REASON_START QTextBoundaryFinder::StartOfItem
#   define BOUNDARY_REASON_END   QTextBoundaryFinder::EndOfItem
#else
#   define BOUNDARY_REASON_START QTextBoundaryFinder::StartWord
#   define BOUNDARY_REASON_END   QTextBoundaryFinder::EndWord
#endif

QString MessageFormatter::formatHtml(const QString& message) const
{
    QString msg = IRC_TEXT_FORMAT.toHtml(message);
    foreach (const QString& user, d.users) {
        int pos = 0;
        while ((pos = msg.indexOf(user, pos)) != -1) {
            QTextBoundaryFinder finder(QTextBoundaryFinder::Word, msg);

            finder.setPosition(pos);
            if (!finder.isAtBoundary() || !finder.boundaryReasons().testFlag(BOUNDARY_REASON_START)) {
                pos += user.length();
                continue;
            }

            finder.setPosition(pos + user.length());
            if (!finder.isAtBoundary() || !finder.boundaryReasons().testFlag(BOUNDARY_REASON_END)) {
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
