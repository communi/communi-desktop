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

#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include <QListView>

class Session;
class UserModel;
class IrcMessage;

class UserListView : public QListView
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel WRITE setChannel)

public:
    UserListView(QWidget* parent = 0);
    ~UserListView();

    Session* session() const;
    void setSession(Session* session);

    QString channel() const;
    void setChannel(const QString& channel);

    UserModel* userModel() const;
    bool hasUser(const QString& user) const;

public slots:
    void processMessage(IrcMessage* message);

private:
    struct UserListViewData
    {
        UserModel* userModel;
    } d;
};

#endif // USERLISTVIEW_H
