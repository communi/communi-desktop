/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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
#include "commandparser.h"
#include "session.h"

AbstractSessionItem::AbstractSessionItem(QObject *parent) :
    QObject(parent), m_session(0), m_busy(false), m_current(false),
    m_highlighted(false), m_unread(0), m_unseen(0)
{
    m_messages = new QStringListModel(this);
    m_formatter.setTimeStamp(true);
    m_formatter.setEventFormat("style='color:gray'");
    m_formatter.setNoticeFormat("style='color:brown'");
    m_formatter.setActionFormat("style='color:darkmagenta'");
    m_formatter.setUnknownFormat("style='color:brown'");
    m_formatter.setHighlightFormat("style='color:#ff3333'");
    m_formatter.setTimeStampFormat("style='color:gray; font-size: small'");
}

AbstractSessionItem::~AbstractSessionItem()
{
    emit removed();
}

Session* AbstractSessionItem::session() const
{
    return m_session;
}

void AbstractSessionItem::setSession(Session *session)
{
    m_session = session;
    m_formatter.setHighlights(QStringList() << session->nickName());
}

QString AbstractSessionItem::title() const
{
    return receiver();
}

void AbstractSessionItem::setTitle(const QString& title)
{
    if (title != receiver())
    {
        setReceiver(title);
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
        setUnreadCount(0);
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

int AbstractSessionItem::unreadCount() const
{
    return m_unread;
}

void AbstractSessionItem::setUnreadCount(int count)
{
    if (!m_current && m_unread != count)
    {
        m_unread = count;
        emit unreadCountChanged();
    }
}

int AbstractSessionItem::unseenIndex() const
{
    return m_unseen;
}

void AbstractSessionItem::setUnseenIndex(int index)
{
    if (m_unseen != index)
    {
        m_unseen = index;
        emit unseenIndexChanged();
    }
}

QObject* AbstractSessionItem::messages() const
{
    return m_messages;
}

bool AbstractSessionItem::hasUser(const QString& user) const
{
    Q_UNUSED(user);
    return false;
}

void AbstractSessionItem::clear()
{
    m_messages->setStringList(QStringList());
}

QStringList AbstractSessionItem::completions(const QString& prefix, const QString& word) const
{
    if (word == "/")
        return CommandParser::availableCommands();
    else if (prefix == "/")
        return CommandParser::availableCommands().filter(QRegExp("^"+word+".*", Qt::CaseInsensitive));
    return QStringList();
}

void AbstractSessionItem::receiveMessage(IrcMessage* message)
{
    const QString formatted = m_formatter.formatMessage(message);
    if (!formatted.isEmpty())
        appendMessage(formatted);
}

const MessageFormatter* AbstractSessionItem::messageFormatter() const
{
    return &m_formatter;
}

void AbstractSessionItem::appendMessage(const QString& message)
{
    const int index = m_messages->rowCount();
    m_messages->insertRow(index);
    m_messages->setData(m_messages->index(index), message);
}
