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

#ifndef SORTERPLUGIN_H
#define SORTERPLUGIN_H

#include <QtPlugin>
#include <QTreeWidgetItem>
#include "treewidgetplugin.h"

class SorterPlugin : public QObject, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    SorterPlugin(QObject* parent = 0);

    void initTree(TreeWidget* tree);
    void cleanupTree(TreeWidget* tree);

    bool eventFilter(QObject* object, QEvent* event);

private:
    void moveSource(QTreeWidgetItem* parent, QTreeWidgetItem* target);

    struct Private {
        TreeWidget* tree;
        QTreeWidgetItem* source;
    } d;
};

#endif // RESETERPLUGIN_H
