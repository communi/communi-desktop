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

#ifndef SESSIONTREEMENU_H
#define SESSIONTREEMENU_H

#include <QMenu>

class SessionTreeItem;
class SessionTreeWidget;

class SessionTreeMenu : public QMenu
{
    Q_OBJECT

public:
    SessionTreeMenu(SessionTreeWidget* parent);

public slots:
    void exec(SessionTreeItem* item, const QPoint& pos);

private slots:
    void onEditTriggered();
    void onWhoisTriggered();
    void onJoinTriggered();
    void onPartTriggered();
    void onCloseTriggered();
    void updateActions();

private:
    void setup(SessionTreeItem* item);

    struct SessionTreePrivate {
        QAction* disconnectAction;
        QAction* reconnectAction;
        QAction* editAction;
        QAction* whoisAction;
        QAction* joinAction;
        QAction* partAction;
        QAction* closeAction;
        SessionTreeItem* item;
    } d;
};

#endif // SESSIONTREEMENU_H
