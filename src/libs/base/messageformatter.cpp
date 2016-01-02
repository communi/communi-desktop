/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "messageformatter.h"
#include <IrcTextFormat>
#include <IrcConnection>
#include <IrcUserModel>
#include <IrcMessage>
#include <IrcPalette>
#include <IrcChannel>
#include <Irc>
#include <QHash>
#include <QTime>
#include <QColor>
#include <QCoreApplication>
#include <QTextBoundaryFinder>

static QString formatSeconds(int secs)
{
    const QDateTime time = QDateTime::fromTime_t(secs);
    return MessageFormatter::tr("%1s").arg(time.secsTo(QDateTime::currentDateTime()));
}

static QString formatDuration(int secs)
{
    QStringList idle;
    if (int days = secs / 86400)
        idle += MessageFormatter::tr("%1 days").arg(days);
    secs %= 86400;
    if (int hours = secs / 3600)
        idle += MessageFormatter::tr("%1 hours").arg(hours);
    secs %= 3600;
    if (int mins = secs / 60)
        idle += MessageFormatter::tr("%1 mins").arg(mins);
    idle += MessageFormatter::tr("%1 secs").arg(secs % 60);
    return idle.join(" ");
}

MessageFormatter::MessageFormatter(QObject* parent) : QObject(parent)
{
    d.buffer = 0;
    d.textFormat = new IrcTextFormat(this);
    d.textFormat->setSpanFormat(IrcTextFormat::SpanClass);

    d.userModel = new IrcUserModel(this);
    connect(d.userModel, SIGNAL(namesChanged(QStringList)), this, SLOT(indexNames(QStringList)));
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

MessageData MessageFormatter::formatMessage(IrcMessage* msg)
{
    QString fmt;
    switch (MessageData::effectiveType(msg)) {
        case IrcMessage::Away:
            fmt = formatAwayMessage(static_cast<IrcAwayMessage*>(msg));
            break;
        case IrcMessage::Invite:
            fmt = formatInviteMessage(static_cast<IrcInviteMessage*>(msg));
            break;
        case IrcMessage::Join:
            fmt = formatJoinMessage(static_cast<IrcJoinMessage*>(msg));
            break;
        case IrcMessage::Kick:
            fmt = formatKickMessage(static_cast<IrcKickMessage*>(msg));
            break;
        case IrcMessage::Mode:
            fmt = formatModeMessage(static_cast<IrcModeMessage*>(msg));
            break;
        case IrcMessage::Motd:
            fmt = formatMotdMessage(static_cast<IrcMotdMessage*>(msg));
            break;
        case IrcMessage::Names:
            fmt = formatNamesMessage(static_cast<IrcNamesMessage*>(msg));
            break;
        case IrcMessage::Nick:
            fmt = formatNickMessage(static_cast<IrcNickMessage*>(msg));
            break;
        case IrcMessage::Notice:
            fmt = formatNoticeMessage(static_cast<IrcNoticeMessage*>(msg));
            break;
        case IrcMessage::Numeric:
            fmt = formatNumericMessage(static_cast<IrcNumericMessage*>(msg));
            break;
        case IrcMessage::Part:
            fmt = formatPartMessage(static_cast<IrcPartMessage*>(msg));
            break;
        case IrcMessage::Pong:
            fmt = formatPongMessage(static_cast<IrcPongMessage*>(msg));
            break;
        case IrcMessage::Private:
            fmt = formatPrivateMessage(static_cast<IrcPrivateMessage*>(msg));
            break;
        case IrcMessage::Quit:
            fmt = formatQuitMessage(static_cast<IrcQuitMessage*>(msg));
            break;
        case IrcMessage::Topic:
            fmt = formatTopicMessage(static_cast<IrcTopicMessage*>(msg));
            break;
        case IrcMessage::Unknown:
            fmt = formatUnknownMessage(msg);
            break;
        case IrcMessage::Whois:
            fmt = formatWhoisMessage(static_cast<IrcWhoisMessage*>(msg));
            break;
        case IrcMessage::Whowas:
            fmt = formatWhowasMessage(static_cast<IrcWhowasMessage*>(msg));
            break;
        case IrcMessage::WhoReply:
            fmt = formatWhoReplyMessage(static_cast<IrcWhoReplyMessage*>(msg));
            break;
        case IrcMessage::Error:
            fmt = formatErrorMessage(static_cast<IrcErrorMessage*>(msg));
            break;
        default:
            break;
    }
    return formatClass(fmt, msg);
}

QString MessageFormatter::formatText(const QString& text) const
{
    d.textFormat->parse(text);

    QString msg = d.textFormat->html();
    if (!d.names.isEmpty()) {
        QTextBoundaryFinder finder = QTextBoundaryFinder(QTextBoundaryFinder::Word, msg);
        int pos = 0;
        while (pos < msg.length()) {
            const QChar c = msg.at(pos);
            if (!c.isSpace()) {
                // do not format nicks within links
                if (c == '<' && msg.midRef(pos, 3) == "<a ") {
                    const int end = msg.indexOf("</a>", pos + 3);
                    if (end != -1) {
                        pos = end + 4;
                        continue;
                    }
                }
                // test word start boundary
                finder.setPosition(pos);
                if (finder.isAtBoundary()) {
                    QMultiHash<QChar, QString>::const_iterator it = d.names.find(c);
                    while (it != d.names.constEnd() && it.key() == c) {
                        const QString& user = it.value();
                        if (msg.midRef(pos, user.length()) == user) {
                            // test word end boundary
                            finder.setPosition(pos + user.length());
                            if (finder.isAtBoundary()) {
                                const QString formatted = QString("<a style='text-decoration:none;' href='nick:%1'>%2</a>").arg(user, styledText(user, Bold | Color));
                                msg.replace(pos, user.length(), formatted);
                                pos += formatted.length();
                                finder = QTextBoundaryFinder(QTextBoundaryFinder::Word, msg);
                            }
                        }
                        ++it;
                    }
                }
            }
            ++pos;
        }
    }
    return msg;
}

QString MessageFormatter::formatExpander(const QString& expander) const
{
    return tr("<a href='expand:' class='event' style='text-decoration:none;'>%1</a>").arg(expander);
}

QString MessageFormatter::styledText(const QString& text, Style style) const
{
    QString fmt = text;
    if (style & Bold)
        fmt = tr("<b>%1</b>").arg(fmt);
    if (style & (Color | Dim)) {
        int bucket = (qHash(text) % 9) + 1;
        if (style & Dim) {
            bucket = 0;
        }
        fmt = tr("<span class='nick%2'>%1</span>").arg(fmt, QString::number(bucket));
    }
    return fmt;
}

QString MessageFormatter::formatAwayMessage(IrcAwayMessage* msg)
{
    if (msg->isOwn())
        return tr("! %1").arg(formatText(msg->content()));
    else if (!msg->content().isEmpty())
        return tr("! %1 is away (%2)").arg(formatSender(msg),
                                           formatText(msg->content()));
    return tr("! %1 is back").arg(formatSender(msg));
}

QString MessageFormatter::formatInviteMessage(IrcInviteMessage* msg)
{
    if (msg->isReply())
        return tr("! invited %1 to %2").arg(styledText(msg->user(), Bold),
                                            styledText(msg->channel(), Bold));

    return tr("%1 %2 invited to %3").arg(formatExpander("!"),
                                         formatSender(msg),
                                         styledText(msg->channel(), Bold));
}

QString MessageFormatter::formatJoinMessage(IrcJoinMessage* msg)
{
    return tr("%1 %2 joined").arg(formatExpander("!"),
                                  formatSender(msg));
}

QString MessageFormatter::formatKickMessage(IrcKickMessage* msg)
{
    return tr("%1 %2 kicked %3").arg(formatExpander("!"),
                                     formatSender(msg),
                                     styledText(msg->user(), Bold));
}

QString MessageFormatter::formatModeMessage(IrcModeMessage* msg)
{
    if (msg->isReply())
        return tr("%1 %2 mode is %3 %4").arg(formatExpander("!"),
                                             styledText(msg->target(), Bold),
                                             styledText(msg->mode(), Bold),
                                             styledText(msg->argument(), Bold));

    return tr("%1 %2 sets mode %3 %4").arg(formatExpander("!"),
                                           formatSender(msg),
                                           styledText(msg->mode(), Bold),
                                           styledText(msg->argument(), Bold));
}

QString MessageFormatter::formatMotdMessage(IrcMotdMessage *msg)
{
    foreach (const QString& line, msg->lines()) {
        MessageData data = formatClass(tr("[MOTD] %1").arg(formatText(line)), msg);
        emit formatted(data);
    }
    return QString();
}

QString MessageFormatter::formatNamesMessage(IrcNamesMessage* msg)
{
    if (msg->flags() & IrcMessage::Implicit)
        return QString();

    if (d.buffer) {
        IrcUserModel userModel(d.buffer);
        userModel.setSortMethod(Irc::SortByTitle);
        QStringList titles = userModel.titles();
        for (int i = 0; i < titles.count(); i += 10) {
            QStringList row = titles.mid(i, 10);
            MessageData data = formatClass(tr("[NAMES] %1").arg(row.join(tr(" "))), msg);
            emit formatted(data);
        }
    }

    return QString();
}

QString MessageFormatter::formatNickMessage(IrcNickMessage* msg)
{
    return tr("%1 %2 changed nick").arg(formatExpander("!"),
                                        styledText(msg->newNick(), Bold));
}

QString MessageFormatter::formatNoticeMessage(IrcNoticeMessage* msg)
{
    if (msg->isReply()) {
        const QStringList params = msg->content().split(" ", QString::SkipEmptyParts);
        const QString cmd = params.value(0);
        if (cmd.toUpper() == "PING") {
            const QString secs = formatSeconds(params.value(1).toInt());
            return tr("! %1 replied in %2").arg(formatSender(msg), secs);
        } else if (cmd.toUpper() == "TIME") {
            const QString rest = QStringList(params.mid(1)).join(" ");
            return tr("! %1 time is %2").arg(formatSender(msg), rest);
        } else if (cmd.toUpper() == "VERSION") {
            const QString rest = QStringList(params.mid(1)).join(" ");
            return tr("! %1 version is %2").arg(formatSender(msg), rest);
        }
    }

    QString pfx = msg->statusPrefix();
    if (!pfx.isEmpty())
        pfx = styledText(":" + pfx, Dim);

    if (msg->isPrivate())
        return tr("[%1%2] %3").arg(formatSender(msg),
                                   pfx,
                                   formatText(msg->content()));

    return tr("&lt;%1%2&gt; [%3] %4").arg(formatSender(msg),
                                          pfx,
                                          msg->target(),
                                          formatText(msg->content()));
}

#define P_(x) msg->parameters().value(x)
#define MID_(x) QStringList(msg->parameters().mid(x)).join(" ")

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* msg)
{
    if (msg->code() < 300)
        return tr("[INFO] %1").arg(formatText(MID_(1)));

    switch (msg->code()) {
        case Irc::RPL_VERSION: // TODO: IrcVersionMessage?
            return tr("! %1 version is %2").arg(styledText(msg->nick(), Bold), P_(1));

        case Irc::RPL_TIME: // TODO: IrcTimeMessage?
            return tr("! %1 time is %2").arg(styledText(P_(1), Bold), P_(2));

        default:
            break;
    }

    if (msg->isComposed() || msg->flags() & IrcMessage::Implicit)
        return QString();

    // if you change this, change formatErrorMessage too
    if (Irc::codeToString(msg->code()).startsWith("ERR_"))
        return tr("[ERROR] %1").arg(formatText(MID_(1)));

    return tr("[%1] %2").arg(msg->code()).arg(d.textFormat->toHtml(MID_(1)));
}

QString MessageFormatter::formatErrorMessage(IrcErrorMessage* msg)
{
    // if you change this, change ERR_ in formatNumericMessage too
    return tr("[ERROR] %1").arg(msg->error());
}

QString MessageFormatter::formatPartMessage(IrcPartMessage* msg)
{
    return tr("%1 %2 left").arg(formatExpander("!"),
                                formatSender(msg));
}

QString MessageFormatter::formatPongMessage(IrcPongMessage* msg)
{
    const QString secs = formatSeconds(msg->argument().toInt());
    return tr("! %1 replied in %2").arg(formatSender(msg), secs);
}

QString MessageFormatter::formatPrivateMessage(IrcPrivateMessage* msg)
{
    if (msg->isRequest())
        return tr("%1 %2 requested %3").arg(formatExpander("!"),
                                            formatSender(msg),
                                            msg->content().split(" ").value(0).toUpper());

    if (msg->isAction())
        return tr("* %1 %2").arg(formatSender(msg),
                                 formatText(msg->content()));

    QString pfx = msg->statusPrefix();
    if (!pfx.isEmpty())
        pfx = styledText(":" + pfx, Dim);

    return tr("&lt;<a style='text-decoration:none;' href='nick:%1'>%2</a>%3&gt; %4").arg(msg->nick(),
                                                                                         formatSender(msg),
                                                                                         pfx,
                                                                                         formatText(msg->content()));
}

QString MessageFormatter::formatQuitMessage(IrcQuitMessage* msg)
{
    QString reason = msg->reason();
    if (reason.contains("Ping timeout")
            || reason.contains("Connection reset by peer")
            || reason.contains("Remote host closed the connection")) {
        return tr("%1 %2 disconnected").arg(formatExpander("!"),
                                            formatSender(msg));
    }
    return tr("%1 %2 quit").arg(formatExpander("!"),
                                formatSender(msg));
}

QString MessageFormatter::formatTopicMessage(IrcTopicMessage* msg)
{
    if (msg->flags() & IrcMessage::Implicit)
        return QString();

    if (msg->isReply()) {
        if (msg->topic().isEmpty())
            return tr("! no topic");
        return tr("[TOPIC] %1").arg(formatText(msg->topic()));
    }

    if (msg->topic().isEmpty())
        return tr("%1 %2 cleared topic").arg(formatExpander("!"),
                                             formatSender(msg));

    return tr("%1 %2 changed topic").arg(formatExpander("!"),
                                         formatSender(msg));
}

QString MessageFormatter::formatUnknownMessage(IrcMessage* msg)
{
    return tr("%1 %2 %3 %4").arg(formatExpander("?"),
                                 formatSender(msg),
                                 msg->command(),
                                 msg->parameters().join(" "));
}

QString MessageFormatter::formatWhoisMessage(IrcWhoisMessage* msg)
{
    emit formatted(formatClass(tr("[WHOIS] %1 is %2@%3 (%4)").arg(msg->nick(), msg->ident(), msg->host(), formatText(msg->realName())), msg));
    emit formatted(formatClass(tr("[WHOIS] %1 is connected via %2 (%3)").arg(msg->nick(), msg->server(), msg->info()), msg));
    emit formatted(formatClass(tr("[WHOIS] %1 is connected since %2 (idle %3)").arg(msg->nick(), msg->since().toString(), formatDuration(msg->idle())), msg));
    if (!msg->awayReason().isEmpty())
        emit formatted(formatClass(tr("[WHOIS] %1 is away: %2").arg(msg->nick(), msg->awayReason()), msg));
    if (!msg->account().isEmpty())
        emit formatted(formatClass(tr("[WHOIS] %1 is logged in as %2").arg(msg->nick(), msg->account()), msg));
    if (!msg->address().isEmpty())
        emit formatted(formatClass(tr("[WHOIS] %1 is connected from %2").arg(msg->nick(), msg->address()), msg));
    if (msg->isSecure())
        emit formatted(formatClass(tr("[WHOIS] %1 is using a secure connection").arg(msg->nick()), msg));
    if (!msg->channels().isEmpty())
        emit formatted(formatClass(tr("[WHOIS] %1 is on %2").arg(msg->nick(), msg->channels().join(" ")), msg));
    return QString();
}

QString MessageFormatter::formatWhowasMessage(IrcWhowasMessage* msg)
{
    emit formatted(formatClass(tr("[WHOWAS] %1 was %2@%3 (%4)").arg(msg->nick(), msg->ident(), msg->host(), formatText(msg->realName())), msg));
    emit formatted(formatClass(tr("[WHOWAS] %1 was connected via %2 (%3)").arg(msg->nick(), msg->server(), msg->info()), msg));
    if (!msg->account().isEmpty())
        emit formatted(formatClass(tr("[WHOWAS] %1 was logged in as %2").arg(msg->nick(), msg->account()), msg));
    return QString();
}

QString MessageFormatter::formatWhoReplyMessage(IrcWhoReplyMessage* msg)
{
    QString format = tr("[WHO] %1 (%2)").arg(formatSender(msg), msg->realName());
    if (msg->isAway())
        format += tr(" - away");
    if (msg->isServOp())
        format += tr(" - server operator");
    return format;
}

MessageData MessageFormatter::formatClass(const QString& format, IrcMessage* msg) const
{
    MessageData data;
    data.initFrom(msg);

    QString cls = "message";
    switch (data.type()) {
        case IrcMessage::Away:
        case IrcMessage::Invite:
        case IrcMessage::Join:
        case IrcMessage::Kick:
        case IrcMessage::Mode:
        case IrcMessage::Motd:
        case IrcMessage::Names:
        case IrcMessage::Nick:
        case IrcMessage::Part:
        case IrcMessage::Pong:
        case IrcMessage::Quit:
        case IrcMessage::Topic:
        case IrcMessage::Whois:
        case IrcMessage::Whowas:
        case IrcMessage::WhoReply:
            cls = "event";
            break;
        case IrcMessage::Unknown:
            cls = "unknown";
            break;
        case IrcMessage::Notice:
            if (IrcNoticeMessage* m = static_cast<IrcNoticeMessage*>(msg))
                cls = m->isReply() ? "event" : "notice";
            break;
        case IrcMessage::Private:
            if (IrcPrivateMessage* m = static_cast<IrcPrivateMessage*>(msg))
                cls = m->isAction() ? "action" : m->isRequest() ? "event" : "message";
            break;
        case IrcMessage::Numeric:
            if (IrcNumericMessage* m = static_cast<IrcNumericMessage*>(msg))
                cls = Irc::codeToString(m->code()).startsWith("ERR_") ? "notice" : "event";
            break;
        case IrcMessage::Error:
            cls = "notice";
            break;
        default:
            break;
    }

    if (!format.isEmpty())
        data.setFormat(tr("<span class='%1'>%2</span>").arg(cls, format));
    return data;
}

QString MessageFormatter::formatSender(IrcMessage* msg) const
{
    Style style = Bold;
    if (MessageData::effectiveType(msg) == IrcMessage::Private) {
        IrcPrivateMessage* p = static_cast<IrcPrivateMessage*>(msg);
        if (!p->isAction() && !p->isRequest())
            style |= msg->isOwn() ? Dim : Color;
    }
    return styledText(msg->nick(), style);
}

void MessageFormatter::indexNames(const QStringList& names)
{
    d.names.clear();
    foreach (const QString& name, names) {
        if (!name.isEmpty())
            d.names.insert(name.at(0), name);
    }
}
