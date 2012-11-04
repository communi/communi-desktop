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

#include "sessionmanager.h"
#include "sessionitem.h"
#include "connectioninfo.h"
#include "settings.h"
#include "session.h"
#include <IrcCommand>

SessionManager::SessionManager(const QString& appName, QDeclarativeContext* context) :
    m_appName(appName), m_context(context)
{
    updateModel();
}

SessionManager::~SessionManager()
{
    save();
    foreach(QObject * item, m_items) {
        SessionItem* sessionItem = static_cast<SessionItem*>(item);
        sessionItem->session()->quit(m_appName);
        delete sessionItem;
    }
    m_items.clear();
}

void SessionManager::addSession(Session* session)
{
    SessionItem* item = new SessionItem(session);
    connect(item, SIGNAL(childAlerted(QObject*, QString)), SIGNAL(alert(QObject*, QString)));
    connect(item, SIGNAL(channelKeyRequired(Session*, QString)), SIGNAL(channelKeyRequired(Session*, QString)));
    m_items.append(item);
    updateModel();
}

void SessionManager::removeSession(Session* session)
{
    for (int i = 0; i < m_items.count(); ++i) {
        SessionItem* item = static_cast<SessionItem*>(m_items.at(i));
        if (item->session() == session) {
            m_items.removeAt(i);
            item->deleteLater();
            updateModel();
            break;
        }
    }
}

void SessionManager::restore()
{
    foreach(const ConnectionInfo & connection, Settings::instance()->connections())
        addSession(Session::fromConnection(connection));
}

void SessionManager::save()
{
    ConnectionInfos connections;
    foreach(QObject * item, m_items) {
        SessionItem* sessionItem = static_cast<SessionItem*>(item);
        connections += sessionItem->session()->toConnection();
    }
    Settings::instance()->setConnections(connections);
}

void SessionManager::updateModel()
{
    if (m_context && m_context->isValid())
        m_context->setContextProperty("SessionModel", QVariant::fromValue(m_items));
}
