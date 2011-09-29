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
*/

#ifndef MESSAGEFORMATTER_H
#define MESSAGEFORMATTER_H

#include <QObject>
#include <IrcMessage>

class MessageFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList highlights READ highlights WRITE setHightlights)
    Q_PROPERTY(bool timeStamp READ timeStamp WRITE setTimeStamp)
    Q_PROPERTY(bool classFormat READ classFormat WRITE setClassFormat)

public:
    explicit MessageFormatter(QObject* parent = 0);
    virtual ~MessageFormatter();

    QStringList highlights() const;
    void setHightlights(const QStringList& highlights);

    bool timeStamp() const;
    void setTimeStamp(bool timeStamp);

    // a workaround for QML Text Element bug: (TODO: QTBUG-XXXXX)
    // nested <span style="color:foo"/> elements cause extra line breaks
    bool classFormat() const;
    void setClassFormat(bool format);

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
    QString formatPongMessage(IrcPongMessage* message) const;
    QString formatPrivateMessage(IrcPrivateMessage* message) const;
    QString formatQuitMessage(IrcQuitMessage* message) const;
    QString formatTopicMessage(IrcTopicMessage* message) const;
    QString formatUnknownMessage(IrcMessage* message) const;

    static QString formatPingReply(const IrcSender& sender, const QString& arg);
    static QString prettyNames(QStringList names, int columns);
    static QString prettyUser(const IrcSender& sender);
    static QString prettyUser(const QString& user);
    static QString colorize(const QString& str);

private:
    mutable struct Private
    {
        bool format;
        bool highlight;
        QStringList highlights;
        bool timeStamp;
        bool firstNames;
        QStringList names;
        QStringList who;
    } d;
};

#endif // MESSAGEFORMATTER_H
