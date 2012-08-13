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

#include "userlistview.h"
#include "sortedusermodel.h"
#include "usermodel.h"
#include "session.h"

UserListView::UserListView(QWidget* parent) : QListView(parent)
{
    d.userModel = new UserModel(this);
}

UserListView::~UserListView()
{
}

Session* UserListView::session() const
{
    return d.userModel->session();
}

void UserListView::setSession(Session* session)
{
    delete model();
    if (session)
        setModel(new SortedUserModel(session->prefixModes(), d.userModel));

    d.userModel->setSession(session);
}

QString UserListView::channel() const
{
    return d.userModel->channel();
}

void UserListView::setChannel(const QString &channel)
{
    d.userModel->setChannel(channel);
}

UserModel* UserListView::userModel() const
{
    return d.userModel;
}

bool UserListView::hasUser(const QString &user) const
{
    return d.userModel->hasUser(user);
}

void UserListView::processMessage(IrcMessage* message)
{
    d.userModel->processMessage(message);
}
