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
#include <QApplication>
#include <IrcSession>
#include <IrcCommand>
#include <Irc>

SessionChildItem::SessionChildItem(SessionItem* parent) :
    AbstractSessionItem(parent), m_parent(parent)
{
    setSession(parent->session());
    connect(this, SIGNAL(titleChanged()), SIGNAL(channelChanged()));
}

bool SessionChildItem::isChannel() const
{
    return title().startsWith('#') || title().startsWith('&');
}

SessionItem* SessionChildItem::sessionItem() const
{
    return m_parent;
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

        QString alertText;
        if (isChannel())
        {
            if (privMsg->message().contains(m_parent->session()->nickName(), Qt::CaseInsensitive))
            {
                setHighlighted(true);
                if (!isCurrent())
                    alertText = tr("%1 on %2:\n%3").arg(privMsg->sender().name()).arg(title()).arg(privMsg->message());
            }
        }
        else
        {
            setHighlighted(true);
            if (!isCurrent())
                alertText = tr("%1 in private:\n%2").arg(privMsg->sender().name()).arg(privMsg->message());
        }

        if (!alertText.isEmpty())
        {
            setAlertText(alertText);
            emit alert(this);
        }

        if (!isCurrent())
            setUnreadCount(unreadCount() + 1);
    }
    else if (message->type() == IrcMessage::Numeric)
    {
        IrcNumericMessage* numMsg = static_cast<IrcNumericMessage*>(message);
        if (isChannel() && numMsg->code() == Irc::RPL_TOPIC)
            setSubtitle(numMsg->parameters().value(2));
        else if (!isChannel() && numMsg->code() == Irc::RPL_WHOISUSER)
            setSubtitle(numMsg->parameters().value(5));
    }
}

void SessionChildItem::close()
{
    if (isChannel())
    {
        static const QString msg = tr("%1 %2 for %2")
                .arg(QApplication::applicationName())
                .arg(QApplication::applicationVersion())
#ifdef Q_OS_SYMBIAN
                .arg(tr("Symbian"));
#else
                .arg(tr("N9"));
#endif
        m_parent->session()->sendCommand(IrcCommand::createPart(title(), msg));
    }
    m_parent->removeChild(title());
}
