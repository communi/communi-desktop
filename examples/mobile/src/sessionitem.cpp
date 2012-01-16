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

#include "sessionitem.h"
#include "sessionchilditem.h"
#include <QApplication>
#include <IrcSession>
#include <IrcMessage>
#include <IrcCommand>
#include <Irc>

SessionItem::SessionItem(Session* session) : AbstractSessionItem(session), m_closing(false)
{
    setTitle(session->host());
    setSubtitle(session->nickName());

    connect(session, SIGNAL(hostChanged(QString)), this, SLOT(setTitle(QString)));
    connect(session, SIGNAL(nickNameChanged(QString)), this, SLOT(setSubtitle(QString)));

    connect(session, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(updateState()));
    connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateState()));
    connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateState()));

    setSession(session);
    m_handler.setSession(session);
    m_handler.setDefaultReceiver(this);
    m_handler.setCurrentReceiver(this);
    connect(&m_handler, SIGNAL(receiverToBeAdded(QString)), SLOT(addChild(QString)));
    connect(&m_handler, SIGNAL(receiverToBeRenamed(QString,QString)), SLOT(renameChild(QString,QString)));
    connect(&m_handler, SIGNAL(receiverToBeRemoved(QString)), SLOT(removeChild(QString)));

    updateCurrent(this);
    updateState();
}

QObjectList SessionItem::childItems() const
{
    return m_children;
}

QStringList SessionItem::channels() const
{
    QStringList chans;
    for (int i = 0; i < m_children.count(); ++i)
    {
        SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
        if (child->isChannel())
            chans += child->title();
    }
    return chans;
}

void SessionItem::updateCurrent(AbstractSessionItem* item)
{
    m_handler.setCurrentReceiver(item->isCurrent() ? item : this);
}

QObject* SessionItem::addChild(const QString& name)
{
    SessionChildItem* child = qobject_cast<SessionChildItem*>(m_handler.getReceiver(name));
    if (!child)
    {
        child = new SessionChildItem(this);
        connect(child, SIGNAL(alert(QObject*)), SIGNAL(alert(QObject*)));
        child->setTitle(name);
        m_handler.addReceiver(name, child);
        m_children.append(child);
        emit childItemsChanged();
    }
    return child;
}

void SessionItem::renameChild(const QString& from, const QString& to)
{
    for (int i = 0; i < m_children.count(); ++i)
    {
        SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
        if (child->title() == from)
        {
            child->setTitle(to);
            emit childItemsChanged();
            break;
        }
    }
}

void SessionItem::removeChild(const QString& name)
{
    m_handler.removeReceiver(name);
    if (title() == name)
    {
        quit();
    }
    else
    {
        for (int i = 0; i < m_children.count(); ++i)
        {
            SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
            if (child->title() == name)
            {
                m_children.takeAt(i)->deleteLater();
                emit childItemsChanged();
                break;
            }
        }
    }
}

void SessionItem::quit()
{
    static const QString msg = tr("%s %s for %s")
            .arg(QApplication::applicationName())
            .arg(QApplication::applicationVersion())
#ifdef Q_OS_SYMBIAN
            .arg(tr("Symbian"));
#else
            .arg(tr("N9"));
#endif
    m_closing = session()->sendCommand(IrcCommand::createQuit(msg));
}

void SessionItem::receiveMessage(IrcMessage* message)
{
    AbstractSessionItem::receiveMessage(message);
    if (message->type() == IrcMessage::Numeric)
    {
        IrcNumericMessage* numeric = static_cast<IrcNumericMessage*>(message);
        if (numeric->code() == Irc::ERR_NICKNAMEINUSE)
            session()->setNickName(session()->nickName() + "_");
        else if (numeric->code() == Irc::ERR_BADCHANNELKEY)
            emit channelKeyRequired(message->parameters().value(1));
    }
}

void SessionItem::updateState()
{
    IrcSession* session = m_handler.session();
    if (session->socket()->error() != QAbstractSocket::UnknownSocketError)
        setIcon("icon-m-transfer-error");
    else if (!session->isConnected() && !session->isActive())
        setIcon("icon-m-common-presence-offline");
    else
        setIcon("icon-m-content-description");

    setBusy(session->isActive() && !session->isConnected());

    bool hasError = session->socket()->error() != QAbstractSocket::UnknownSocketError;
    if (!m_closing && hasError)
        setAlertText(QString("%1\n%2").arg(session->host()).arg(session->socket()->errorString()));

    if (m_closing && (hasError || !session->isConnected()))
        session->deleteLater();
}
