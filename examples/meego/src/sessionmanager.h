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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDeclarativeContext>
#include <QNetworkSession>
class SessionItem;
class Session;

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool online READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(bool offline READ isOffline NOTIFY offlineStateChanged)

public:
    SessionManager(QDeclarativeContext* context);

    bool isOnline() const;
    bool isOffline() const;

    Q_INVOKABLE void addSession(Session* session);
    Q_INVOKABLE void removeSession(Session* session);
    Q_INVOKABLE bool ensureNetwork();

public slots:
    void restore();
    void save();

signals:
    void onlineStateChanged();
    void offlineStateChanged();
    void alert(QObject* item);

private slots:
    void onNetworkStateChanged(QNetworkSession::State state);

private:
    void updateModel();
    QObjectList m_items;
    QDeclarativeContext* m_context;
    QNetworkSession* m_network;
};

#endif // SESSIONMANAGER_H
