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

#include "sessionitem.h"
#include "sessionmodel.h"
#include "session.h"

SessionItem::SessionItem(SessionModel* model) : QObject(model)
{
    connect(model->session(), SIGNAL(activeChanged(bool)), this, SLOT(onActiveChanged()));
}

SessionItem::~SessionItem()
{
}

Session* SessionItem::session() const
{
    if (SessionModel* m = model())
        return m->session();
    return 0;
}

SessionModel* SessionItem::model() const
{
    return qobject_cast<SessionModel*>(parent());
}

QString SessionItem::name() const
{
    return d.name;
}

void SessionItem::setName(const QString& name)
{
    if (d.name != name) {
        d.name = name;
        emit nameChanged(name);
    }
}

bool SessionItem::isActive() const
{
    return session()->isActive();
}

bool SessionItem::hasUser(const QString& user) const
{
    return !session()->nickName().compare(user, Qt::CaseInsensitive);
}

void SessionItem::receiveMessage(IrcMessage* message)
{
    emit messageReceived(message);
}

void SessionItem::onActiveChanged()
{
    emit activeChanged(isActive());
}
