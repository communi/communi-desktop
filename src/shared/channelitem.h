/*
* Copyright (C) 2008-2013 Communi authors
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

#ifndef CHANNELITEM_H
#define CHANNELITEM_H

#include "sessionitem.h"
#include "usermodel.h"

class ChannelItem : public SessionItem
{
    Q_OBJECT
    Q_PROPERTY(QString topic READ topic NOTIFY topicChanged)
    Q_PROPERTY(UserModel* userModel READ userModel CONSTANT)

public:
    ChannelItem(const QString& name, SessionModel* model);
    ~ChannelItem();

    QString topic() const;
    UserModel* userModel() const;

    virtual bool isActive() const;
    virtual bool hasUser(const QString& user) const;
    virtual void receiveMessage(IrcMessage* message);

signals:
    void topicChanged(const QString& topic);

private:
    struct Private {
        UserModel* userModel;
        QString topic;
        bool joined;
    } d;
};

#endif // CHANNELITEM_H
