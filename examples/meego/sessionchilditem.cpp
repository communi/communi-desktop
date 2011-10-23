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

#include "sessionchilditem.h"
#include "sessionitem.h"
#include <IrcSession>
#include <Irc>

SessionChildItem::SessionChildItem(SessionItem* parent) :
    AbstractSessionItem(parent), m_parent(parent)
{
    setSession(parent->session());
    connect(this, SIGNAL(titleChanged()), SLOT(updateIcon()));
}

void SessionChildItem::updateCurrent(AbstractSessionItem* item)
{
    m_parent->updateCurrent(item);
}

void SessionChildItem::receiveMessage(IrcMessage* message)
{
    AbstractSessionItem::receiveMessage(message);
    if (message->type() == IrcMessage::Private)
    {
        IrcPrivateMessage* privMsg = static_cast<IrcPrivateMessage*>(message);

        QString info;
        if (title().startsWith('#') || title().startsWith('&'))
        {
            if (privMsg->message().contains(m_parent->session()->nickName(), Qt::CaseInsensitive))
            {
                setHighlighted(true);
                if (!isCurrent())
                    info = tr("%1 on %2:\n%3").arg(privMsg->sender().name()).arg(title()).arg(privMsg->message());
            }
        }
        else
        {
            setHighlighted(true);
            if (!isCurrent())
                info = tr("%1 in private:\n%2").arg(privMsg->sender().name()).arg(privMsg->message());
        }

        if (!info.isEmpty())
            emit alert(info);

        if (!isCurrent())
            setUnread(unread() + 1);
    }
    else if (message->type() == IrcMessage::Numeric)
    {
        IrcNumericMessage* numMsg = static_cast<IrcNumericMessage*>(message);
        if (numMsg->code() == Irc::RPL_TOPIC)
            setSubtitle(numMsg->parameters().value(2));
    }
}

void SessionChildItem::updateIcon()
{
    if (title().startsWith('#') || title().startsWith('&'))
        setIcon("icon-m-conversation-group-chat");
    else
        setIcon("icon-m-content-avatar-placeholder");
}
