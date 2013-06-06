/*
* Copyright (C) 2008-2013 The Communi Project
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

#ifndef SESSION_H
#define SESSION_H

#include <QSet>
#include <QTimer>
#include <IrcSession>
#include <IrcCommand>
#include <IrcLagTimer>
#include <QStringList>
#include <IrcSessionInfo>
#include <QAbstractSocket>
#include <IrcMessageFilter>
#include <QElapsedTimer>
#include "viewinfo.h"

class Session : public IrcSession, public IrcMessageFilter
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int autoReconnectDelay READ autoReconnectDelay WRITE setAutoReconnectDelay)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(bool reconnecting READ isReconnecting)
    Q_PROPERTY(bool hasQuit READ hasQuit WRITE setHasQuit)

public:
    explicit Session(QObject* parent = 0);
    ~Session();

    QString name() const;
    void setName(const QString& name);

    IrcLagTimer* lagTimer() const;

    int autoReconnectDelay() const;
    void setAutoReconnectDelay(int delay);

    Q_INVOKABLE bool isChannel(const QString& receiver) const;
    Q_INVOKABLE QString userPrefix(const QString& user) const;
    Q_INVOKABLE QString unprefixedUser(const QString& user) const;

    QString password() const;
    void setPassword(const QString& password);

    bool hasQuit() const;
    void setHasQuit(bool quit);

    bool isReconnecting() const;

    ViewInfos views() const;
    void setViews(const ViewInfos& views);

    Q_INVOKABLE bool sendUiCommand(IrcCommand* command, const QString& identifier);

public slots:
    void reconnect();
    void quit();
    void destructLater();
    void stopReconnecting();
    void sleep();
    void wake();

signals:
    void nameChanged(const QString& name);

private slots:
    void onConnected();
    void onDisconnected();
    void onPassword(QString* password);
    void onCapabilities(const QStringList& available, QStringList* request);
    void applySettings();

private:
    bool messageFilter(IrcMessage* message);
    void addChannel(const QString& channel);
    void removeChannel(const QString& channel);

    struct Private {
        QString name;
        QTimer reconnectTimer;
        QString password;
        ViewInfos views;
        bool quit;
        QStringList alternateNicks;
        long timestamp;
        QElapsedTimer timestamper;
        IrcLagTimer* lag;
        int rejoin;
        QHash<QString, IrcCommand*> commands;
    } d;
};

#endif // SESSION_H
