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

#ifndef MENUPLUGIN_H
#define MENUPLUGIN_H

#include <QMenu>
#include <QtPlugin>
#include <QTreeWidget>
#include "treeplugin.h"

class MenuPlugin : public QMenu, public TreePlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin)
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")

public:
    MenuPlugin(QObject* parent = 0);

    void initialize(TreeWidget* tree);

    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void exec(QTreeWidgetItem* item, const QPoint& pos);

private slots:
    void onEditTriggered();
    void onWhoisTriggered();
    void onJoinTriggered();
    void onPartTriggered();
    void onCloseTriggered();
    void updateActions();

private:
    void setup(QTreeWidgetItem* item);

    struct Private {
        TreeWidget* tree;
        QAction* disconnectAction;
        QAction* reconnectAction;
        QAction* editAction;
        QAction* whoisAction;
        QAction* joinAction;
        QAction* partAction;
        QAction* closeAction;
        QTreeWidgetItem* item;
    } d;
};

#endif // MENUPLUGIN_H
