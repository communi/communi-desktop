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

#include "abstractsessionitem.h"
#include <IrcSession>

AbstractSessionItem::AbstractSessionItem(QObject *parent) :
    QObject(parent), m_session(0), m_busy(false), m_current(false),
    m_highlighted(false), m_unread(false), m_unseen(false)
{
    m_messages = new QStringListModel(this);
    m_formatter.setTimeStamp(true);
    m_formatter.setEventFormat("style='color:gray'");
    m_formatter.setNoticeFormat("style='color:brown'");
    m_formatter.setActionFormat("style='color:darkmagenta'");
    m_formatter.setUnknownFormat("style='color:brown'");
    m_formatter.setHighlightFormat("style='color:red'");
}

IrcSession* AbstractSessionItem::session() const
{
    return m_session;
}

void AbstractSessionItem::setSession(IrcSession *session)
{
    m_session = session;
    m_formatter.setHighlights(QStringList() << session->nickName());
}

QString AbstractSessionItem::icon() const
{
    return m_icon;
}

void AbstractSessionItem::setIcon(const QString& icon)
{
    if (m_icon != icon)
    {
        m_icon = icon;
        emit iconChanged();
    }
}
QString AbstractSessionItem::title() const
{
    return m_title;
}

void AbstractSessionItem::setTitle(const QString& title)
{
    if (m_title != title)
    {
        m_title = title;
        emit titleChanged();
    }
}

QString AbstractSessionItem::subtitle() const
{
    return m_subtitle;
}

void AbstractSessionItem::setSubtitle(const QString& subtitle)
{
    if (m_subtitle != subtitle)
    {
        m_subtitle = subtitle;
        emit subtitleChanged();
    }
}

bool AbstractSessionItem::isBusy() const
{
    return m_busy;
}

void AbstractSessionItem::setBusy(bool busy)
{
    if (m_busy != busy)
    {
        m_busy = busy;
        emit busyChanged();
    }
}

bool AbstractSessionItem::isCurrent() const
{
    return m_current;
}

void AbstractSessionItem::setCurrent(bool current)
{
    if (current)
    {
        setHighlighted(false);
        setUnread(0);
    }
    else
    {
        setUnseen(0);
    }

    if (m_current != current)
    {
        m_current = current;
        emit currentChanged();
        updateCurrent(this);
    }
}

bool AbstractSessionItem::isHighlighted() const
{
    return m_highlighted;
}

void AbstractSessionItem::setHighlighted(bool highlighted)
{
    if (!m_current && m_highlighted != highlighted)
    {
        m_highlighted = highlighted;
        emit highlightedChanged();
    }
}

int AbstractSessionItem::unread() const
{
    return m_unread;
}

void AbstractSessionItem::setUnread(int unread)
{
    if (!m_current && m_unread != unread)
    {
        m_unread = unread;
        emit unreadChanged();
    }
}

int AbstractSessionItem::unseen() const
{
    return m_unseen;
}

void AbstractSessionItem::setUnseen(int unseen)
{
    if (!m_current && m_unseen != unseen)
    {
        m_unseen = unseen;
        emit unseenChanged();
    }
}

QStringList AbstractSessionItem::users() const
{
    return m_users;
}

QObject* AbstractSessionItem::messages() const
{
    return m_messages;
}

void AbstractSessionItem::addUser(const QString& user)
{
    m_users.append(user);
    emit usersChanged();
}

void AbstractSessionItem::removeUser(const QString& user)
{
    m_users.removeOne(user);
    emit usersChanged();
}

void AbstractSessionItem::receiveMessage(IrcMessage* message)
{
    const QString formatted = m_formatter.formatMessage(message);
    if (!formatted.isEmpty())
    {
        const int index = m_messages->rowCount();
        m_messages->insertRow(index);
        m_messages->setData(m_messages->index(index), formatted);

        if (!m_current)
            setUnseen(m_unseen + 1);
    }
}
