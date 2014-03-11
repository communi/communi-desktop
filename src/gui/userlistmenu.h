/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef USERLISTMENU_H
#define USERLISTMENU_H

#include <QMenu>

class ListView;

class UserListMenu : public QMenu
{
    Q_OBJECT

public:
    UserListMenu(ListView* parent);

public slots:
    void exec(const QPoint& pos);

private slots:
    void onWhoisTriggered();
    void onQueryTriggered();
    void onModeTriggered();
    void onKickTriggered();
    void onBanTriggered();

private:
    struct Private {
        ListView* view;
    } d;
};

#endif // USERLISTMENU_H
