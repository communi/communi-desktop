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

#include "sessionmanager.h"
#include "session.h"
#include <QNetworkConfigurationManager>

SessionManager::SessionManager(QObject* parent) : QObject(parent)
{
    QNetworkConfigurationManager manager;
    d.network = new QNetworkSession(manager.defaultConfiguration(), this);
    connect(d.network, SIGNAL(stateChanged(QNetworkSession::State)), this, SLOT(onNetworkStateChanged(QNetworkSession::State)));
    connect(d.network, SIGNAL(stateChanged(QNetworkSession::State)), this, SIGNAL(onlineStateChanged()));
    connect(d.network, SIGNAL(stateChanged(QNetworkSession::State)), this, SIGNAL(offlineStateChanged()));
}

bool SessionManager::isOnline() const
{
    return d.network->state() == QNetworkSession::Connected;
}

bool SessionManager::isOffline() const
{
    return d.network->state() != QNetworkSession::Connected;
}

void SessionManager::addSession(Session* session)
{
    session->setUserName("communi");
    d.sessions.append(session);
    if (!d.network->isOpen())
        d.network->open();
    session->open();
    emit sessionAdded(session);
}

void SessionManager::removeSession(Session* session)
{
    if (d.sessions.removeOne(session))
        emit sessionRemoved(session);
}

void SessionManager::onNetworkStateChanged(QNetworkSession::State state)
{
    if (state == QNetworkSession::Connected)
    {
        foreach (Session* session, d.sessions)
        {
            if (session && !session->isActive())
                session->open();
        }
    }
    else if (state == QNetworkSession::Closing)
    {
        foreach (Session* session, d.sessions)
        {
            if (session && session->isActive())
                session->close();
        }
    }
}
