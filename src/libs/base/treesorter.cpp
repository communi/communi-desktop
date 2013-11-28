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

#include "treesorter.h"
#include "treeitem.h"
#include "treewidget.h"
#include <QSettings>

bool TreeSorter::enabled = true;
QStringList TreeSorter::parents;
QHashStringList TreeSorter::children;

bool TreeSorter::isEnabled()
{
    return enabled;
}

void TreeSorter::setEnabled(TreeWidget* tree, bool value)
{
    if (enabled != value) {
        enabled = value;
        tree->setSortingEnabled(value);
    }
}

void TreeSorter::init(TreeWidget* tree)
{
    parents.clear();
    children.clear();
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QStringList lst;
        QTreeWidgetItem* parent = tree->topLevelItem(i);
        for (int j = 0; j < parent->childCount(); ++j)
            lst += parent->child(j)->text(0);
        children.insert(parent->text(0), lst);
        parents += parent->text(0);
    }
}

void TreeSorter::save()
{
    QSettings settings;
    settings.beginGroup("Sorting");
    QHash<QString, QVariant> variants;
    QHashIterator<QString, QStringList> it(children);
    while (it.hasNext()) {
        it.next();
        variants.insert(it.key(), it.value());
    }
    settings.setValue("children", variants);
    settings.setValue("parents", parents);
}

void TreeSorter::restore()
{
    QSettings settings;
    settings.beginGroup("Sorting");
    children.clear();
    QHashIterator<QString, QVariant> it(settings.value("children").toHash());
    while (it.hasNext()) {
        it.next();
        children.insert(it.key(), it.value().toStringList());
    }
    parents = settings.value("parents").toStringList();
}

bool TreeSorter::sort(const TreeItem* one, const TreeItem* another)
{
    QStringList order;
    const TreeItem* parent = one->parentItem();
    if (!parent)
        order = parents;
    else if (enabled)
        order = children.value(parent->text(0));
    const int oidx = order.indexOf(one->text(0));
    const int aidx = order.indexOf(another->text(0));
    if (oidx == -1  || aidx == -1)
        return standardTreeSortFunc(one, another);
    return oidx < aidx;
}
