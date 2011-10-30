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

#include <QTimer>
#include <IrcSession>
#include <QAbstractSocket>

class Session : public IrcSession
{
    Q_OBJECT
    Q_PROPERTY(bool secure READ isSecure WRITE setSecure)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword)

public:
    explicit Session(QObject *parent = 0);

    int autoReconnectDelay() const;
    void setAutoReconnectDelay(int delay);

    bool isSecure() const;
    void setSecure(bool secure);

    QString getPassword() const;
    void setPassword(const QString& password);

private slots:
    void onPassword(QString* password);

private:
    QTimer m_timer;
    QString m_password;
};

#endif // SESSION_H
