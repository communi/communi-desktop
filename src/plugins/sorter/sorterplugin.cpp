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

#include "sorterplugin.h"
#include "treewidget.h"
#include "treeitem.h"
#include <QMouseEvent>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QHash>

typedef QHash<QString, QStringList> QHashStringList;

Q_GLOBAL_STATIC(QStringList, topLevelSortOrder)
Q_GLOBAL_STATIC(QHashStringList, childSortOrders)

bool CustomTreeSorter(const TreeItem* one, const TreeItem* another)
{
    QStringList order;
    const TreeItem* parent = one->parentItem();
    if (!parent)
        order = *topLevelSortOrder();
    else
        order = childSortOrders()->value(parent->text(0));
    const int oidx = order.indexOf(one->text(0));
    const int aidx = order.indexOf(another->text(0));
    if (oidx == -1 && aidx == -1)
        return DefaultTreeSorter(one, another);
    if (oidx != -1 && aidx == -1)
        return true;
    if (aidx != -1 && oidx == -1)
        return false;
    return oidx < aidx;
}

SorterPlugin::SorterPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.action = 0;
}

void SorterPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
    d.action = new QAction(tr("Sort"), tree);
    connect(d.action, SIGNAL(toggled(bool)), this, SLOT(toggleSorting(bool)));
    d.action->setCheckable(true);
    d.tree->addAction(d.action);

    QSettings settings;
    settings.beginGroup("Sorting");
    toggleSorting(settings.value("enabled", true).toBool());

    d.tree->viewport()->installEventFilter(this);
    connect(tree, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(onPressed(QTreeWidgetItem*)));
}

void SorterPlugin::uninitialize(TreeWidget* tree)
{
    QSettings settings;
    settings.beginGroup("Sorting");
    settings.setValue("enabled", d.tree->isSortingEnabled());

    tree->viewport()->removeEventFilter(this);
}

bool SorterPlugin::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (d.source) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            QTreeWidgetItem* target = d.tree->itemAt(me->pos());
            if (target && target->parent() && d.source != target && d.source->parent() == target->parent()) {
                setSortingEnabled(false);
                QTreeWidgetItem* parent = target->parent();
                int idx = parent->indexOfChild(target);
                parent->takeChild(parent->indexOfChild(d.source));
                parent->insertChild(idx, d.source);
                saveOrder();
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            d.source = 0;
        }
    }
    return false;
}

void SorterPlugin::toggleSorting(bool enabled)
{
    if (!enabled)
        restoreOrder();
    setSortingEnabled(enabled);
    if (!enabled)
        d.tree->sortByColumn(0, Qt::AscendingOrder);
}

void SorterPlugin::setSortingEnabled(bool enabled)
{
    const bool wasEnabled = d.tree->isSortingEnabled();
    d.tree->setSorter(enabled ? DefaultTreeSorter : CustomTreeSorter);
    d.tree->setSortingEnabled(enabled);
    if (enabled && !wasEnabled)
        d.tree->sortByColumn(0, Qt::AscendingOrder);

    d.action->blockSignals(true);
    d.action->setChecked(enabled);
    d.action->blockSignals(false);
}

void SorterPlugin::onPressed(QTreeWidgetItem* item)
{
    d.source = item;
}

void SorterPlugin::saveOrder()
{
    QSettings settings;
    settings.beginGroup("Sorting");
    QStringList parents;
    QHash<QString, QVariant> children;
    for (int i = 0; i < d.tree->topLevelItemCount(); ++i) {
        QStringList lst;
        QTreeWidgetItem* parent = d.tree->topLevelItem(i);
        for (int j = 0; j < parent->childCount(); ++j)
            lst += parent->child(j)->text(0);
        children.insert(parent->text(0), lst);
        parents += parent->text(0);
    }
    settings.setValue("children", children);
    settings.setValue("parents", parents);
}

void SorterPlugin::restoreOrder()
{
    QSettings settings;
    settings.beginGroup("Sorting");
    QHashIterator<QString, QVariant> it(settings.value("children").toHash());
    while (it.hasNext()) {
        it.next();
        *childSortOrders()->insert(it.key(), it.value().toStringList());
    }
    *topLevelSortOrder() = settings.value("parents").toStringList();
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(SorterPlugin)
#endif
