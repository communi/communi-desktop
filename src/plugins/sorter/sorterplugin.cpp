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

bool CustomTreeSorter(const TreeItem* one, const TreeItem* another);

SorterPlugin::SorterPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.action = 0;
}

void SorterPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
    d.action = new QAction(tr("Sort"), tree);
    connect(d.action, SIGNAL(toggled(bool)), this, SLOT(setSortingEnabled(bool)));
    d.action->setCheckable(true);
    d.action->setChecked(true);
    setSortingEnabled(true);
    d.tree->addAction(d.action);

    d.tree->viewport()->installEventFilter(this);
    connect(tree, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(onPressed(QTreeWidgetItem*)));
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
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            d.source = 0;
        }
    }
    return false;
}

void SorterPlugin::setSortingEnabled(bool enabled)
{
    d.action->setChecked(enabled);
    d.tree->setSortingEnabled(enabled);
    d.tree->setSorter(enabled ? DefaultTreeSorter : CustomTreeSorter);
}

void SorterPlugin::onPressed(QTreeWidgetItem* item)
{
    d.source = item;
}

bool CustomTreeSorter(const TreeItem* one, const TreeItem* another)
{
    // TODO:
    return DefaultTreeSorter(one, another);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(SorterPlugin)
#endif
