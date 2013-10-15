/*
* Copyright (C) 2008-2013 The Communi Project
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "messageformatter.h"
#include <IrcTextFormat>
#include <IrcConnection>
#include <IrcUserModel>
#include <IrcChannel>
#include <Irc>
#include <QHash>
#include <QTime>
#include <QColor>
#include <QCoreApplication>
#include <QTextBoundaryFinder>

MessageFormatter::MessageFormatter(QObject* parent) : QObject(parent)
{
    d.buffer = 0;
    d.highlighted = false;
    d.timeStampFormat = "[hh:mm:ss]";
    d.userModel = new IrcUserModel(this);
    d.textFormat = new IrcTextFormat(this);
}

IrcBuffer* MessageFormatter::buffer() const
{
    return d.buffer;
}

void MessageFormatter::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        d.buffer = buffer;
        d.userModel->setChannel(qobject_cast<IrcChannel*>(buffer));
    }
}

IrcTextFormat* MessageFormatter::textFormat() const
{
    return d.textFormat;
}

void MessageFormatter::setTextFormat(IrcTextFormat* format)
{
    d.textFormat = format;
}

bool MessageFormatter::wasHighlighted() const
{
    return d.highlighted;
}

QString MessageFormatter::formatMessage(IrcMessage* message)
{
    QString formatted;
    d.highlighted = false;
    switch (message->type()) {
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
        case IrcMessage::Names:
            formatted = formatNamesMessage(static_cast<IrcNamesMessage*>(message));
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
        case IrcMessage::Pong:
            formatted = formatPongMessage(static_cast<IrcPongMessage*>(message));
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
        default:
            break;
    }
    return formatLine(formatted, message->timeStamp());
}

QString MessageFormatter::formatLine(const QString& message, const QDateTime& timeStamp)
{
    QString formatted = message;
    if (formatted.isEmpty())
        return QString();

    QString cls = "message";
    if (d.highlighted) {
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

    if (!d.timeStampFormat.isEmpty())
        formatted = QCoreApplication::translate("MessageFormatter", "<span class='timestamp'>%1</span> %3").arg(timeStamp.time().toString(d.timeStampFormat), formatted);

    return QCoreApplication::translate("MessageFormatter", "<span class='%1'>%2</span>").arg(cls, formatted);
}

QString MessageFormatter::formatInviteMessage(IrcInviteMessage* message)
{
    const QString sender = formatSender(message->prefix());
    return QCoreApplication::translate("MessageFormatter", "! %1 invited to %3").arg(sender, message->channel());
}

QString MessageFormatter::formatJoinMessage(IrcJoinMessage* message)
{
    const bool repeat = d.repeats.value(d.buffer);
    const QString sender = formatSender(message->prefix(), false);
    if (message->flags() & IrcMessage::Own && repeat)
        return QCoreApplication::translate("MessageFormatter", "! %1 rejoined %2").arg(sender, message->channel());
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 joined %2").arg(sender, message->channel());
}

QString MessageFormatter::formatKickMessage(IrcKickMessage* message)
{
    const QString sender = formatSender(message->prefix());
    const QString user = formatUser(message->user());
    if (!message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 kicked %2 (%3)").arg(sender, user, message->reason());
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 kicked %2").arg(sender, user);
}

QString MessageFormatter::formatModeMessage(IrcModeMessage* message)
{
    const bool repeat = d.repeats.value(d.buffer);
    const QString sender = formatSender(message->prefix());
    if (message->isReply())
        return !repeat ? QCoreApplication::translate("MessageFormatter", "! %1 mode is %2 %3").arg(message->target(), message->mode(), message->argument()) : QString();
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 sets mode %2 %3").arg(sender, message->mode(), message->argument());
}

QString MessageFormatter::formatNamesMessage(IrcNamesMessage* message)
{
    const bool repeat = d.repeats.value(d.buffer);
    if (!repeat && !message->names().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 has %2 users").arg(message->channel()).arg(message->names().count());
    return QString();
}

QString MessageFormatter::formatNickMessage(IrcNickMessage* message)
{
    const QString sender = formatSender(message->prefix());
    const QString nick = formatUser(message->nick());
    return QCoreApplication::translate("MessageFormatter", "! %1 changed nick to %2").arg(sender, nick);
}

QString MessageFormatter::formatNoticeMessage(IrcNoticeMessage* message)
{
    if (message->content().contains(d.buffer->connection()->nickName()))
        d.highlighted = true;

    if (message->isReply()) {
        const QStringList params = message->content().split(" ", QString::SkipEmptyParts);
        const QString cmd = params.value(0);
        const QString arg = params.value(1);
        if (cmd.toUpper() == "PING")
            return formatPingReply(message->prefix(), arg);
        else if (cmd.toUpper() == "TIME")
            return QCoreApplication::translate("MessageFormatter", "! %1 time is %2").arg(formatSender(message->prefix()), QStringList(params.mid(1)).join(" "));
        else if (cmd.toUpper() == "VERSION")
            return QCoreApplication::translate("MessageFormatter", "! %1 version is %2").arg(formatSender(message->prefix()), QStringList(params.mid(1)).join(" "));
    }

    const QString sender = formatSender(message->prefix());
    const QString msg = formatHtml(message->content());
    return QCoreApplication::translate("MessageFormatter", "[%1] %2").arg(sender, msg);
}

#define P_(x) message->parameters().value(x)
#define MID_(x) QStringList(message->parameters().mid(x)).join(" ")

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* message)
{
    const bool repeat = d.repeats.value(d.buffer);
    if (message->code() < 300)
        return !repeat ? QCoreApplication::translate("MessageFormatter", "[INFO] %1").arg(formatHtml(MID_(1))) : QString();

    switch (message->code()) {
        case Irc::RPL_MOTDSTART:
        case Irc::RPL_MOTD:
            if (!repeat)
                return QCoreApplication::translate("MessageFormatter", "[MOTD] %1").arg(formatHtml(MID_(1)));
            return QString();
        case Irc::RPL_ENDOFMOTD:
            if (repeat)
                return QCoreApplication::translate("MessageFormatter", "! %1 reconnected").arg(d.buffer->connection()->nickName());
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
            return QCoreApplication::translate("MessageFormatter", "! %1 is %2@%3 (%4)").arg(P_(1), P_(2), P_(3), formatHtml(MID_(5)));
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
            return !repeat ? QCoreApplication::translate("MessageFormatter", "! %1 url is %2").arg(P_(1), formatHtml(P_(2))) : QString();
        case Irc::RPL_CREATIONTIME:
            if (!repeat) {
                QDateTime dateTime = QDateTime::fromTime_t(P_(2).toInt());
                return QCoreApplication::translate("MessageFormatter", "! %1 was created %2").arg(P_(1), dateTime.toString());
            }
            return QString();
        case Irc::RPL_TOPICWHOTIME:
            if (!repeat) {
                QDateTime dateTime = QDateTime::fromTime_t(P_(3).toInt());
                return QCoreApplication::translate("MessageFormatter", "! %1 topic was set %2 by %3").arg(P_(1), dateTime.toString(), formatUser(P_(2)));
            }
            return QString();

        case Irc::RPL_INVITING:
            return QCoreApplication::translate("MessageFormatter", "! inviting %1 to %2").arg(formatUser(P_(1)), P_(2));
        case Irc::RPL_VERSION:
            return QCoreApplication::translate("MessageFormatter", "! %1 version is %2").arg(formatSender(message->prefix()), P_(1));
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
            if (Irc::codeToString(message->code()).startsWith("ERR_"))
                return QCoreApplication::translate("MessageFormatter", "[ERROR] %1").arg(formatHtml(MID_(1)));

            return QCoreApplication::translate("MessageFormatter", "[%1] %2").arg(message->code()).arg(QStringList(message->parameters().mid(1)).join(" "));
    }
}

QString MessageFormatter::formatPartMessage(IrcPartMessage* message)
{
    const QString sender = formatSender(message->prefix(), false);
    if (!message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 parted %2 (%3)").arg(sender, message->channel(), formatHtml(message->reason()));
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 parted %2").arg(sender, message->channel());
}

QString MessageFormatter::formatPongMessage(IrcPongMessage* message)
{
    return formatPingReply(message->prefix(), message->argument());
}

QString MessageFormatter::formatPrivateMessage(IrcPrivateMessage* message)
{
    const QString content = message->content();

    if (message->nick() == QLatin1String("***") && message->ident() == QLatin1String("znc")) {
        if (content == QLatin1String("Buffer Playback..."))
            d.playbacks[d.buffer] = true;
        else if (content == QLatin1String("Playback Complete."))
            d.playbacks[d.buffer] = false;
    }

    if (content.contains(d.buffer->connection()->nickName()))
        d.highlighted = true;

    const QString sndr = formatSender(message->prefix());
    const QString msg = formatHtml(content);
    if (message->isAction())
        return QCoreApplication::translate("MessageFormatter", "* %1 %2").arg(sndr, msg);
    else if (message->isRequest())
        return QCoreApplication::translate("MessageFormatter", "! %1 requested %2").arg(sndr, msg.split(" ").value(0).toLower());
    else
        return QCoreApplication::translate("MessageFormatter", "&lt;%1&gt; %2").arg(sndr, msg);
}

QString MessageFormatter::formatQuitMessage(IrcQuitMessage* message)
{
    const QString sender = formatSender(message->prefix(), false);
    if (!message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "! %1 has quit (%2)").arg(sender, formatHtml(message->reason()));
    else
        return QCoreApplication::translate("MessageFormatter", "! %1 has quit").arg(sender);
}

QString MessageFormatter::formatTopicMessage(IrcTopicMessage* message)
{
    const bool repeat = d.repeats.value(d.buffer);
    const QString sender = formatSender(message->prefix());
    const QString topic = formatHtml(message->topic());
    const QString channel = message->channel();
    if (message->isReply()) {
        if (topic.isEmpty())
            return !repeat ? QCoreApplication::translate("MessageFormatter", "! %1 has no topic set").arg(channel) : QString();
        return !repeat ? QCoreApplication::translate("MessageFormatter", "! %1 topic is \"%2\"").arg(channel, topic) : QString();
    }
    return QCoreApplication::translate("MessageFormatter", "! %1 sets topic \"%2\" on %3").arg(sender, topic, channel);
}

QString MessageFormatter::formatUnknownMessage(IrcMessage* message)
{
    const QString sender = formatSender(message->prefix());
    return QCoreApplication::translate("MessageFormatter", "? %1 %2 %3").arg(sender, message->command(), message->parameters().join(" "));
}

QString MessageFormatter::formatPingReply(const QString& sender, const QString& arg)
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

QString MessageFormatter::formatSender(const QString& sender, bool strip)
{
    QString name = Irc::nickFromPrefix(sender);
    if (!name.isEmpty()) {
        QColor color = QColor::fromHsl(qHash(name) % 359, 255, 64);
        name = QString("<b><a href='nick:%2' style='text-decoration:none; color:%1'>%2</a></b>").arg(color.name()).arg(name);
        if (!strip) {
            QString ident = Irc::identFromPrefix(sender);
            QString host = Irc::hostFromPrefix(sender);
            if (!ident.isEmpty() && !host.isEmpty())
                name = QString("%1 (%2@%3)").arg(name, ident, host);
        }
    } else {
        name = sender;
    }
    return name;
}

QString MessageFormatter::formatUser(const QString& user, bool strip)
{
    return formatSender(user, strip);
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

QString MessageFormatter::formatHtml(const QString& message)
{
    QString msg = d.textFormat->toHtml(message);
    const QStringList names = d.userModel->names();
    for (int i = names.count() - 1; i >= 0; --i) {
        const QString& user = names.at(i);
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
