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

#ifndef MESSAGEFORMATTER_H
#define MESSAGEFORMATTER_H

#include <QHash>
#include <QObject>
#include <IrcMessage>

class MessageFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool highlight READ hasHighlight)
    Q_PROPERTY(QStringList highlights READ highlights WRITE setHighlights)
    Q_PROPERTY(QStringList users READ users WRITE setUsers)
    Q_PROPERTY(bool timeStamp READ timeStamp WRITE setTimeStamp)
    Q_PROPERTY(bool stripNicks READ stripNicks WRITE setStripNicks)
    Q_PROPERTY(QString timeStampFormat READ timeStampFormat WRITE setTimeStampFormat)

public:
    explicit MessageFormatter(QObject* parent = 0);
    virtual ~MessageFormatter();

    bool hasHighlight() const;

    QStringList highlights() const;
    void setHighlights(const QStringList& highlights);

    QStringList users() const;
    void setUsers(const QStringList& users);

    bool timeStamp() const;
    void setTimeStamp(bool timeStamp);

    bool stripNicks() const;
    void setStripNicks(bool strip);

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    Q_INVOKABLE QString formatMessage(IrcMessage* message) const;
    Q_INVOKABLE QString formatMessage(const QDateTime& timeStamp, const QString& message) const;

    Q_INVOKABLE QString formatHtml(const QString& message) const;

protected:
    QString formatInviteMessage(IrcInviteMessage* message) const;
    QString formatJoinMessage(IrcJoinMessage* message) const;
    QString formatKickMessage(IrcKickMessage* message) const;
    QString formatModeMessage(IrcModeMessage* message) const;
    QString formatNamesMessage(IrcNamesMessage* message) const;
    QString formatNickMessage(IrcNickMessage* message) const;
    QString formatNoticeMessage(IrcNoticeMessage* message) const;
    QString formatNumericMessage(IrcNumericMessage* message) const;
    QString formatPartMessage(IrcPartMessage* message) const;
    QString formatPongMessage(IrcPongMessage* message) const;
    QString formatPrivateMessage(IrcPrivateMessage* message) const;
    QString formatQuitMessage(IrcQuitMessage* message) const;
    QString formatTopicMessage(IrcTopicMessage* message) const;
    QString formatUnknownMessage(IrcMessage* message) const;

    static QString formatPingReply(const IrcSender& sender, const QString& arg);

    static QString formatSender(const IrcSender& sender, bool strip = true);
    static QString formatUser(const QString& user, bool strip = true);

    static QString formatIdleTime(int secs);

private:
    mutable struct Private {
        int conns;
        int joins;
        bool highlight;
        bool timeStamp;
        bool stripNicks;
        QStringList users;
        QStringList highlights;
        QString timeStampFormat;
    } d;
};

#endif // MESSAGEFORMATTER_H
