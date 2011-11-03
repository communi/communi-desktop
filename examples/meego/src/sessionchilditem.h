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

#ifndef SESSIONCHILDITEM_H
#define SESSIONCHILDITEM_H

#include "abstractsessionitem.h"

class SessionItem;
class IrcMessage;

class SessionChildItem : public AbstractSessionItem
{
    Q_OBJECT
    Q_PROPERTY(bool channel READ isChannel CONSTANT)
    Q_PROPERTY(SessionItem* sessionItem READ sessionItem)

public:
    explicit SessionChildItem(SessionItem* parent);

    bool isChannel() const;
    SessionItem* sessionItem() const;

    void updateCurrent(AbstractSessionItem* item);

public slots:
    void close();

protected slots:
    void receiveMessage(IrcMessage* message);

private slots:
    void updateIcon();

private:
    SessionItem* m_parent;
};

#endif // SESSIONCHILDITEM_H
