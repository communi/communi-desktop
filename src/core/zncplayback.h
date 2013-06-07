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

#ifndef ZNCPLAYBACK_H
#define ZNCPLAYBACK_H

#include <QObject>
#include <IrcMessageFilter>

class IrcNoticeMessage;
class IrcPrivateMessage;

class ZncPlayback : public QObject, public IrcMessageFilter
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive)
    Q_PROPERTY(QString target READ target)
    Q_PROPERTY(QString timeStampFormat READ timeStampFormat WRITE setTimeStampFormat)

public:
    explicit ZncPlayback(QObject* parent = 0);
    virtual ~ZncPlayback();

    bool isActive() const;
    QString target() const;

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    bool messageFilter(IrcMessage* message);

signals:
    void messagePlayed(IrcMessage* message);
    void targetChanged(const QString& target, bool active);

protected:
    bool processMessage(IrcPrivateMessage* message);
    bool processNotice(IrcNoticeMessage* message);

private:
    mutable struct Private {
        bool active;
        QString target;
        QString timeStampFormat;
    } d;
};

#endif // ZNCPLAYBACK_H
