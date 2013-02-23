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

#include "channelitem.h"
#include <IrcMessage>

ChannelItem::ChannelItem(const QString& name, SessionModel* model) : SessionItem(model)
{
    setName(name);
    d.joined = false;

    d.userModel = new UserModel(this);
    d.userModel->setSession(session());
    d.userModel->setChannel(name);
    connect(this, SIGNAL(nameChanged(QString)), d.userModel, SLOT(setChannel(QString)));
}

ChannelItem::~ChannelItem()
{
}

UserModel* ChannelItem::userModel() const
{
    return d.userModel;
}

bool ChannelItem::isActive() const
{
    return SessionItem::isActive() && d.joined;
}

bool ChannelItem::hasUser(const QString& user) const
{
    return SessionItem::hasUser(user) || d.userModel->hasUser(user);
}

void ChannelItem::receiveMessage(IrcMessage* message)
{
    d.userModel->processMessage(message);

    switch (message->type()) {
        case IrcMessage::Join:
            if (message->flags() & IrcMessage::Own) {
                d.joined = true;
                emit activeChanged(isActive());
            }
            break;
        case IrcMessage::Part:
            if (message->flags() & IrcMessage::Own) {
                d.joined = false;
                emit activeChanged(isActive());
            }
            break;
        default:
            break;
    }

    SessionItem::receiveMessage(message);
}
