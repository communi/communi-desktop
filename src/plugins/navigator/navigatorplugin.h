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

#ifndef NAVIGATORPLUGIN_H
#define NAVIGATORPLUGIN_H

#include <QtPlugin>
#include <QShortcut>
#include <QTreeWidget>
#include "treeplugin.h"

class NavigatorPlugin : public QObject, public TreePlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin)
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")

public:
    explicit NavigatorPlugin(QObject* parent = 0);

    void setTree(TreeWidget* tree);

    QTreeWidgetItem* currentItem() const;
    void setCurrentItem(QTreeWidgetItem* item);

    QTreeWidgetItem* topLevelItem(int index) const;

    void expandItem(QTreeWidgetItem* item);
    void collapseItem(QTreeWidgetItem* item);

    QTreeWidgetItem* lastItem() const;
    QTreeWidgetItem* nextItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* previousItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* findNextItem(QTreeWidgetItem* from, int column, int role) const;
    QTreeWidgetItem* findPrevItem(QTreeWidgetItem* from, int column, int role) const;

public slots:
    void moveToNextItem();
    void moveToPrevItem();

    void moveToNextActiveItem();
    void moveToPrevActiveItem();

    void expandCurrentConnection();
    void collapseCurrentConnection();
    void moveToMostActiveItem();

private:
    struct Private {
        TreeWidget* tree;
        QShortcut* prevShortcut;
        QShortcut* nextShortcut;
        QShortcut* prevActiveShortcut;
        QShortcut* nextActiveShortcut;
        QShortcut* expandShortcut;
        QShortcut* collapseShortcut;
        QShortcut* mostActiveShortcut;
    } d;
};

#endif // NAVIGATORPLUGIN_H
