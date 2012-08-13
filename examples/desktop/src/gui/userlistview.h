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

class UserListView : public QListView
{
    Q_OBJECT

public:
    UserListView(QWidget* parent = 0);
    ~UserListView();

private:
    struct UserListViewData
    {
    } d;
};

#endif // USERLISTVIEW_H
