/*
  Copyright (C) 2008-2014 The Communi Project

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

#include "eventformatter.h"

EventFormatter::EventFormatter(QObject* parent) : MessageFormatter(parent)
{
}

QString EventFormatter::formatEvent(const QString& event) const
{
    return tr("<span class='event'>%1 %2</span>").arg(formatExpander("!"), event);
}

QString EventFormatter::formatInviteMessage(IrcInviteMessage* msg)
{
    return tr("! %1 invited to %2").arg(formatSender(msg),
                                        styledText(msg->channel(), Bold));
}

QString EventFormatter::formatJoinMessage(IrcJoinMessage* msg)
{
    return tr("! %1 joined").arg(formatSender(msg));
}

QString EventFormatter::formatKickMessage(IrcKickMessage* msg)
{
    if (msg->reason().isEmpty() || msg->reason() == msg->user())
        return tr("! %1 kicked %2").arg(formatSender(msg),
                                        styledText(msg->user(), Bold|Color));

    return tr("! %1 kicked %2 (%3)").arg(formatSender(msg),
                                         styledText(msg->user(), Bold|Color),
                                         formatText(msg->reason()));
}

QString EventFormatter::formatModeMessage(IrcModeMessage* msg)
{
    if (msg->isReply())
        return tr("! %1 mode is %2 %3").arg(styledText(msg->target(), Bold),
                                            styledText(msg->mode(), Bold),
                                            styledText(msg->argument(), Bold));

    return tr("! %1 sets mode %2 %3").arg(formatSender(msg),
                                          styledText(msg->mode(), Bold),
                                          styledText(msg->argument(), Bold));
}

QString EventFormatter::formatNickMessage(IrcNickMessage* msg)
{
    return tr("! %1 changed nick to %2").arg(formatSender(msg),
                                             styledText(msg->newNick(), Bold|Color));
}

QString EventFormatter::formatNoticeMessage(IrcNoticeMessage* msg)
{
    return MessageFormatter::formatNoticeMessage(msg);
}

QString EventFormatter::formatNumericMessage(IrcNumericMessage* msg)
{
    return MessageFormatter::formatNumericMessage(msg);
}

QString EventFormatter::formatPartMessage(IrcPartMessage* msg)
{
    if (msg->reason().isEmpty() || msg->reason() == msg->nick())
        return tr("! %1 left").arg(formatSender(msg));

    return tr("! %1 left (%2)").arg(formatSender(msg),
                                    formatText(msg->reason()));
}

QString EventFormatter::formatPongMessage(IrcPongMessage* msg)
{
    return MessageFormatter::formatPongMessage(msg);
}

QString EventFormatter::formatPrivateMessage(IrcPrivateMessage* msg)
{
    if (msg->isRequest())
        return tr("! %1 requested %2").arg(formatSender(msg),
                                           msg->content().split(" ").value(0).toUpper());

    if (msg->isAction())
        return tr("* %1 %2").arg(formatSender(msg),
                                 formatText(msg->content()));

    return tr("&lt;%1&gt; %2").arg(formatSender(msg),
                                   formatText(msg->content()));
}

QString EventFormatter::formatQuitMessage(IrcQuitMessage* msg)
{
    if (msg->reason().isEmpty() || msg->reason() == msg->nick())
        return tr("! %1 quit").arg(formatSender(msg));

    return tr("! %1 quit (%2)").arg(formatSender(msg),
                                    formatText(msg->reason()));
}

QString EventFormatter::formatTopicMessage(IrcTopicMessage* msg)
{
    if (msg->isReply())
        return QString();

    if (msg->topic().isEmpty())
        return tr("! %1 cleared topic").arg(formatSender(msg));

    return tr("! %1 changed topic to \"%2\"").arg(formatSender(msg),
                                                  formatText(msg->topic()));
}

QString EventFormatter::formatUnknownMessage(IrcMessage* msg)
{
    return tr("? %2 %3 %4").arg(formatSender(msg),
                                msg->command(),
                                msg->parameters().join(" "));
}

QString EventFormatter::formatSender(IrcMessage* msg) const
{
    QString prefix = styledText(msg->nick(), Bold | (msg->isOwn() ? Dim : Color));
    if (!msg->ident().isEmpty() && !msg->host().isEmpty())
        return tr("%1&nbsp;(%2@%3)").arg(prefix, msg->ident(), msg->host());
    return styledText(msg->nick(), Bold|Color);
}
