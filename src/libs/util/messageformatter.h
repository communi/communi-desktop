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

#ifndef MESSAGEFORMATTER_H
#define MESSAGEFORMATTER_H

#include <QHash>
#include <QDateTime>
#include <IrcMessage>
#include <IrcTextFormat>

class IrcBuffer;
class IrcUserModel;

class MessageFormatter : public QObject
{
    Q_OBJECT

public:
    MessageFormatter(QObject* parent = 0);

    IrcBuffer* buffer() const;
    void setBuffer(IrcBuffer* buffer);

    IrcTextFormat* textFormat() const;
    void setTextFormat(IrcTextFormat* format);

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    Q_INVOKABLE QString formatMessage(IrcMessage* message);
    QString formatLine(const QString& message, const QDateTime& timeStamp = QDateTime::currentDateTime());
    QString formatHtml(const QString& message);

protected:
    QString formatInviteMessage(IrcInviteMessage* message);
    QString formatJoinMessage(IrcJoinMessage* message);
    QString formatKickMessage(IrcKickMessage* message);
    QString formatModeMessage(IrcModeMessage* message);
    QString formatNamesMessage(IrcNamesMessage* message);
    QString formatNickMessage(IrcNickMessage* message);
    QString formatNoticeMessage(IrcNoticeMessage* message);
    QString formatNumericMessage(IrcNumericMessage* message);
    QString formatPartMessage(IrcPartMessage* message);
    QString formatPongMessage(IrcPongMessage* message);
    QString formatPrivateMessage(IrcPrivateMessage* message);
    QString formatQuitMessage(IrcQuitMessage* message);
    QString formatTopicMessage(IrcTopicMessage* message);
    QString formatUnknownMessage(IrcMessage* message);

    static QString formatPingReply(const QString& nick, const QString& arg);

    static QString formatNick(const QString& nick, bool own = false);
    static QString formatPrefix(const QString& prefix, bool strip = true, bool own = false);

    static QString formatIdleTime(int secs);

    static QString formatNames(const QStringList& names, int columns = 6);

private:
    struct Private {
        IrcBuffer* buffer;
        IrcUserModel* userModel;
        QString timeStampFormat;
        IrcTextFormat* textFormat;
        QHash<IrcBuffer*, bool> repeats;
    } d;
};

#endif // MESSAGEFORMATTER_H
