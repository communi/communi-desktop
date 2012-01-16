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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include "connection.h"

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(int port READ port WRITE setPort)
    Q_PROPERTY(bool secure READ isSecure WRITE setSecure)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString channel READ channel WRITE setChannel)
    Q_PROPERTY(Connections connections READ connections WRITE setConnections)

public:
    explicit Settings(QObject *parent = 0);

    QString host() const;
    void setHost(const QString& host);

    int port() const;
    void setPort(int port);

    bool isSecure() const;
    void setSecure(bool secure);

    QString name() const;
    void setName(const QString& name);

    QString channel() const;
    void setChannel(const QString& channel);

    Connections connections() const;
    void setConnections(const Connections& connections);
};

#endif // SETTINGS_H
