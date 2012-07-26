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
#include "usermodel.h"
#include <QDateTime>
#include <IrcSession>
#include <IrcMessage>
#include <Irc>

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
    m_formatter.setHighlightFormat("style='color:red'");
}

AbstractSessionItem::~AbstractSessionItem()
{
    emit removed();
    delete m_usermodel;
}

Session* AbstractSessionItem::session() const
{
    return m_session;
}

void AbstractSessionItem::setSession(Session *session)
{
    m_session = session;
    m_formatter.setPrefixes(session->prefixModes());
    m_formatter.setHighlights(QStringList() << session->nickName());
    m_usermodel = new UserModel(session);
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

QString AbstractSessionItem::alertText() const
{
    return m_alertText;
}

void AbstractSessionItem::setAlertText(const QString& text)
{
    m_alertText = text;
}

QStringList AbstractSessionItem::users() const
{
    QStringList users;
    for (int i = 0; i < m_usermodel->rowCount(); ++i)
        users += m_usermodel->data(m_usermodel->index(i)).toString();
    return users;
}

QObject* AbstractSessionItem::messages() const
{
    return m_messages;
}

bool AbstractSessionItem::hasUser(const QString& user) const
{
    return m_usermodel->hasUser(user);
}

void AbstractSessionItem::addUser(const QString& user)
{
    m_usermodel->addUser(user);
    emit usersChanged();
}

void AbstractSessionItem::addUsers(const QStringList& users)
{
    m_usermodel->addUsers(users);
    emit usersChanged();
}

void AbstractSessionItem::removeUser(const QString& user)
{
    m_usermodel->removeUser(user);
    emit usersChanged();
}

void AbstractSessionItem::renameUser(const QString &from, const QString &to)
{
    m_usermodel->renameUser(from, to);
    emit usersChanged();
}

void AbstractSessionItem::setUserMode(const QString& user, const QString& mode)
{
    m_usermodel->setUserMode(user, mode);
    emit usersChanged();
}

void AbstractSessionItem::sendUiCommand(IrcCommand *command)
{
    m_sent.insert(command->type());
    m_session->sendUiCommand(command);
}

void AbstractSessionItem::clear()
{
    m_messages->setStringList(QStringList());
}

void AbstractSessionItem::receiveMessage(IrcMessage* message)
{
    if (message->type() == IrcMessage::Numeric)
    {
        IrcNumericMessage* numeric = static_cast<IrcNumericMessage*>(message);
        switch (numeric->code())
        {
        case Irc::RPL_ENDOFNAMES:
            if (m_sent.contains(IrcCommand::Names))
            {
                emit namesReceived(m_formatter.currentNames());
                m_sent.remove(IrcCommand::Names);
                m_formatter.formatMessage(message);
                return;
            }
            break;
        case Irc::RPL_WHOISUSER:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("Ident: %1").arg(message->parameters().value(2)));
                m_whois.append(tr("Host: %1").arg(message->parameters().value(3)));
                m_whois.append(tr("Name: %1").arg(message->parameters().value(5)));
                return;
            }
            break;
        case Irc::RPL_WHOISSERVER:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("Server: %1 (%2)").arg(message->parameters().value(2), message->parameters().value(3)));
                return;
            }
            break;
        case Irc::RPL_WHOISOPERATOR:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("IRC operator"));
                return;
            }
            break;
        case Irc::RPL_WHOISACCOUNT:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("Logged in as: %1").arg(message->parameters().value(2)));
                return;
            }
            break;
        case Irc::RPL_WHOISREGNICK:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("Registered nick"));
                return;
            }
            break;
        case Irc::RPL_WHOISSECURE:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("Secure connection"));
                return;
            }
            break;
        case Irc::RPL_WHOISIDLE:
            if (m_sent.contains(IrcCommand::Whois))
            {
                QDateTime signon = QDateTime::fromTime_t(message->parameters().value(3).toInt());
                QTime idle = QTime().addSecs(message->parameters().value(2).toInt());
                m_whois.append(tr("Connected: %1").arg(signon.toString()));
                m_whois.append(tr("Idle: %1").arg(idle.toString()));
                return;
            }
            break;
        case Irc::RPL_WHOISCHANNELS:
            if (m_sent.contains(IrcCommand::Whois))
            {
                m_whois.append(tr("Channels: %1").arg(message->parameters().value(2)));
                return;
            }
            break;
        case Irc::RPL_WHOISHOST:
        case Irc::RPL_WHOISMODES:
            if (m_sent.contains(IrcCommand::Whois))
            {
                return;
            }
            break;
        case Irc::RPL_ENDOFWHOIS:
            if (m_sent.contains(IrcCommand::Whois))
            {
                emit whoisReceived(m_whois);
                m_sent.remove(IrcCommand::Whois);
                m_whois.clear();
            }
        case Irc::RPL_WHOISHELPOP:
        case Irc::RPL_WHOISSPECIAL:
            return;
        default:
            break;
        }
    }

    const QString formatted = m_formatter.formatMessage(message);
    if (!formatted.isEmpty())
    {
        const int index = m_messages->rowCount();
        m_messages->insertRow(index);
        m_messages->setData(m_messages->index(index), formatted);
    }
}
