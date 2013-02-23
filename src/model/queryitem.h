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

#ifndef QUERYITEM_H
#define QUERYITEM_H

#include "sessionitem.h"

class QueryItem : public SessionItem
{
    Q_OBJECT

public:
    QueryItem(const QString& name, SessionModel* model);
    ~QueryItem();

    virtual bool hasUser(const QString& user) const;
    virtual void receiveMessage(IrcMessage* message);
};

#endif // QUERYITEM_H
