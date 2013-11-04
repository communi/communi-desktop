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

#ifndef TREEMENU_H
#define TREEMENU_H

#include <QMenu>
#include <QPointer>

class TreeItem;
class TreeWidget;

class TreeMenu : public QMenu
{
    Q_OBJECT

public:
    TreeMenu(TreeWidget* tree);

    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void onEditTriggered();
    void onWhoisTriggered();
    void onJoinTriggered();
    void onPartTriggered();
    void onCloseTriggered();
    void updateActions();

private slots:
    void setup(TreeItem* item);

private:
    struct Private {
        TreeWidget* tree;
        QPointer<TreeItem> item;
        QAction* disconnectAction;
        QAction* reconnectAction;
        QAction* editAction;
        QAction* whoisAction;
        QAction* joinAction;
        QAction* partAction;
        QAction* closeAction;
    } d;
};

#endif // MENUPLUGIN_H
