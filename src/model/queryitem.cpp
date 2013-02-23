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

#include "queryitem.h"

QueryItem::QueryItem(const QString& name, SessionModel* model) : SessionItem(model)
{
    setName(name);
}

QueryItem::~QueryItem()
{
}

bool QueryItem::hasUser(const QString& user) const
{
    return SessionItem::hasUser(user) || !name().compare(user, Qt::CaseInsensitive);
}

void QueryItem::receiveMessage(IrcMessage* message)
{
    SessionItem::receiveMessage(message);
}
