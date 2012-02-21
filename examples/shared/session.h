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

#ifndef SESSION_H
#define SESSION_H

#include <QSet>
#include <QTimer>
#include <IrcSession>
#include <QStringList>
#include <QElapsedTimer>
#include <QAbstractSocket>
#include <QNetworkSession>
#include "connection.h"

class Session : public IrcSession
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString network READ network NOTIFY networkChanged)
    Q_PROPERTY(int autoReconnectDelay READ autoReconnectDelay WRITE setAutoReconnectDelay)
    Q_PROPERTY(QStringList channels READ channels WRITE setChannels)
    Q_PROPERTY(bool secure READ isSecure WRITE setSecure)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(int pingInterval READ pingInterval WRITE setPingInterval)
    Q_PROPERTY(int currentLag READ currentLag NOTIFY currentLagChanged)
    Q_PROPERTY(int maximumLag READ maximumLag WRITE setMaximumLag)

public:
    explicit Session(QObject *parent = 0);

    QString name() const;
    void setName(const QString& name);

    QString network() const;

    int autoReconnectDelay() const;
    void setAutoReconnectDelay(int delay);

    QStringList channels() const;
    void setChannels(const QStringList& channels);

    bool isSecure() const;
    void setSecure(bool secure);

    QString password() const;
    void setPassword(const QString& password);

    Connection toConnection() const;
    static Session* fromConnection(const Connection& connection, QObject* parent = 0);

    int pingInterval() const;
    void setPingInterval(int interval);

    int currentLag() const;
    int maximumLag() const;
    void setMaximumLag(int lag);

    Q_INVOKABLE bool ensureNetwork();

public slots:
    void reconnect();
    void quit(const QString& reason = QString());
    void destructLater();

signals:
    void nameChanged(const QString& name);
    void networkChanged(const QString& network);
    void currentLagChanged(int lag);

private slots:
    void onConnected();
    void onPassword(QString* password);
    void handleMessage(IrcMessage* message);
    void togglePingTimer(bool enabled);
    void pingServer();

private:
    void updateLag(int lag);

    QString m_name;
    QTimer m_reconnectTimer;
    QString m_password;
    QStringList m_channels;
    QElapsedTimer m_lagTimer;
    QTimer m_pingTimer;
    int m_currentLag;
    int m_maxLag;
    QHash<QString,QString> m_info;
    static QNetworkSession* s_network;
};

#endif // SESSION_H
