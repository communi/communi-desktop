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
#include "sorter.h"
#include <QMouseEvent>

SorterPlugin::SorterPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.source = 0;
}

void SorterPlugin::initTree(TreeWidget* tree)
{
    d.tree = tree;
    Sorter::restore();
    tree->setSortFunc(&Sorter::sort);
    tree->viewport()->installEventFilter(this);
}

void SorterPlugin::cleanupTree(TreeWidget* tree)
{
    tree->viewport()->removeEventFilter(this);
}

bool SorterPlugin::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        d.source = d.tree->itemAt(me->pos());
    } else if (d.source) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            QTreeWidgetItem* target = d.tree->itemAt(me->pos());
            if (target && d.source != target) {
                QTreeWidgetItem* parent = target->parent();
                if (parent == d.source->parent()) {
                    Sorter::setEnabled(d.tree, false);
                    moveSource(parent ? parent : d.tree->invisibleRootItem(), target);
                }
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            if (d.source && !Sorter::isEnabled()) {
                Sorter::init(d.tree);
                Sorter::save();
            }
            Sorter::setEnabled(d.tree, true);
            d.source = 0;
        }
    }
    return false;
}

void SorterPlugin::moveSource(QTreeWidgetItem* parent, QTreeWidgetItem* target)
{
    const bool se = d.source->isExpanded();
    const bool te = target->isExpanded();
    const int idx = parent->indexOfChild(target);
    parent->takeChild(parent->indexOfChild(d.source));
    parent->insertChild(idx, d.source);
    d.source->setExpanded(se);
    target->setExpanded(te);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(SorterPlugin)
#endif
