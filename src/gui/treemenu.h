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

#ifndef TREEMENU_H
#define TREEMENU_H

#include <QMenu>

class TreeItem;
class TreeWidget;

class TreeMenu : public QMenu
{
    Q_OBJECT

public:
    TreeMenu(TreeWidget* parent);

public slots:
    void exec(TreeItem* item, const QPoint& pos);

private slots:
    void onEditTriggered();
    void onWhoisTriggered();
    void onJoinTriggered();
    void onPartTriggered();
    void onCloseTriggered();
    void updateActions();

private:
    void setup(TreeItem* item);

    struct Private {
        QAction* disconnectAction;
        QAction* reconnectAction;
        QAction* editAction;
        QAction* whoisAction;
        QAction* joinAction;
        QAction* partAction;
        QAction* closeAction;
        TreeItem* item;
    } d;
};

#endif // TREEMENU_H
