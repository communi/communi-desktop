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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSet>
#include <QTimer>
#include <IrcConnection>
#include <IrcCommand>
#include <QStringList>
#include <IrcNetwork>
#include <QAbstractSocket>
#include <IrcMessageFilter>
#include "viewinfo.h"

class Connection : public IrcConnection, public IrcMessageFilter
{
    Q_OBJECT
    Q_INTERFACES(IrcMessageFilter)
    Q_PROPERTY(bool bouncer READ isBouncer)

public:
    explicit Connection(QObject* parent = 0);
    ~Connection();

    bool isBouncer() const;

    ViewInfos views() const;
    void setViews(const ViewInfos& views);

    Q_INVOKABLE bool sendUiCommand(IrcCommand* command, const QString& identifier);

public slots:
    void quit();

protected:
    IrcCommand* createCtcpReply(IrcPrivateMessage* request) const;

private slots:
    void onConnected();
    void onNickNameReserved(QString* alternate);

private:
    bool messageFilter(IrcMessage* message);
    void addChannel(const QString& channel);
    void removeChannel(const QString& channel);

    struct Private {
        bool bouncer;
        ViewInfos views;
        QStringList alternateNicks;
        QHash<QString, IrcCommand*> commands;
    } d;
};

#endif // CONNECTION_H
