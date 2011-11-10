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
#include <QAbstractSocket>
#include "connection.h"

class Session : public IrcSession
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int autoReconnectDelay READ autoReconnectDelay WRITE setAutoReconnectDelay)
    Q_PROPERTY(QStringList channels READ channels WRITE setChannels)
    Q_PROPERTY(bool secure READ isSecure WRITE setSecure)
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    explicit Session(QObject *parent = 0);

    QString name() const;
    void setName(const QString& name);

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

private slots:
    void onConnected();
    void onPassword(QString* password);
    void handleMessage(IrcMessage* message);

private:
    QString m_name;
    QTimer m_timer;
    QString m_password;
    QSet<QString> m_channels;
};

#endif // SESSION_H
