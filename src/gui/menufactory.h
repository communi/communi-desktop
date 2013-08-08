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

#ifndef MENUFACTORY_H
#define MENUFACTORY_H

#include <QMenu>

class UserListView;

class MenuFactory : public QObject
{
    Q_OBJECT

public:
    MenuFactory(QObject* parent);
    virtual ~MenuFactory();

    virtual QMenu* createUserListMenu(const QString& prefix, const QString& name, UserListView* listView);
};

#endif // MENUFACTORY_H
