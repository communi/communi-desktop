/*
* Copyright (C) 2008-2013 Communi authors
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

#ifndef SESSIONITEM_H
#define SESSIONITEM_H

#include <QString>
#include <QObject>

class IrcMessage;
class SessionModel;
class Session;

class SessionItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    SessionItem(SessionModel* model);
    ~SessionItem();

    Session* session() const;
    SessionModel* model() const;

    QString name() const;
    void setName(const QString& name);

    virtual bool isActive() const;
    virtual bool hasUser(const QString& user) const;
    virtual void receiveMessage(IrcMessage* message);

signals:
    void activeChanged(bool active);
    void nameChanged(const QString& name);
    void messageReceived(IrcMessage* message);

private slots:
    void onActiveChanged();

private:
    struct Private {
        SessionModel* model;
        QString name;
    } d;
};

#endif // SESSIONITEM_H
