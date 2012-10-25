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

#ifndef MENUFACTORY_H
#define MENUFACTORY_H

#include <QMenu>

class MessageView;
class UserListView;
class SessionTreeItem;
class SessionTabWidget;
class SessionTreeWidget;

class MenuFactory : public QObject
{
    Q_OBJECT

public:
    MenuFactory(QObject* parent);
    virtual ~MenuFactory();

    virtual QMenu* createUserViewMenu(const QString& user, MessageView* view);
    virtual QMenu* createTabViewMenu(MessageView* view, SessionTabWidget* tab);
    virtual QMenu* createUserListMenu(const QString& user, UserListView* listView);
    virtual QMenu* createSessionTreeMenu(SessionTreeItem* item, SessionTreeWidget* tree);
};

#endif // MENUFACTORY_H
