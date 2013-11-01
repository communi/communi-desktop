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

#ifndef RESETER_H
#define RESETER_H

#include <QtPlugin>
#include <QShortcut>
#include <QTreeWidget>
#include "treeplugin.h"

class Reseter : public QObject, public TreePlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin)
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")

public:
    Reseter(QObject* parent = 0);

    void initialize(TreeWidget* tree);

    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void delayedReset(QTreeWidgetItem* item);
    void resetItems();

private:
    struct Private {
        bool blocked;
        TreeWidget* tree;
        QShortcut* shortcut;
    } d;
    friend class TreeItem;
};

#endif // RESETER_H
