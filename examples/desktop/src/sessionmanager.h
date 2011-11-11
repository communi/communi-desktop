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

#include <QPointer>
#include <QNetworkSession>
class Session;

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool online READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(bool offline READ isOffline NOTIFY offlineStateChanged)

public:
    SessionManager(QObject* parent = 0);

    bool isOnline() const;
    bool isOffline() const;

    Q_INVOKABLE void addSession(Session* session);
    Q_INVOKABLE void removeSession(Session* session);

signals:
    void onlineStateChanged();
    void offlineStateChanged();

    void sessionAdded(Session* session);
    void sessionRemoved(Session* session);

private slots:
    void onNetworkStateChanged(QNetworkSession::State state);

private:
    struct Private
    {
        QList<QPointer<Session> > sessions;
        QNetworkSession* network;
    } d;
};

#endif // SESSIONMANAGER_H
