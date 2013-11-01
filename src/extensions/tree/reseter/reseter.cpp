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

#include "reseter.h"
#include <QEvent>

Reseter::Reseter(QObject* parent) : QObject(parent)
{
    d.blocked = false;
    d.tree = 0;
    d.shortcut = 0;
}

void Reseter::initialize(QTreeWidget* tree)
{
    d.tree = tree;

    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    d.shortcut = new QShortcut(tree);
    d.shortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(d.shortcut, SIGNAL(activated()), this, SLOT(resetItems()));
}

bool Reseter::eventFilter(QObject *object, QEvent* event)
{
    Q_UNUSED(object);
    switch (event->type()) {
    case QEvent::WindowActivate:
        delayedReset(d.tree->currentItem());
        break;
    case QEvent::WindowBlocked:
        d.blocked = true;
        break;
    case QEvent::WindowUnblocked:
        d.blocked = false;
        delayedReset(d.tree->currentItem());
        break;
    default:
        break;
    }
    return false;
}

void Reseter::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.blocked) {
        //if (previous)
            // TODO: static_cast<TreeItem*>(previous)->reset();
    }
    if (current)
        delayedReset(current);
}

void Reseter::delayedReset(QTreeWidgetItem* item)
{
    //if (item)
        // TODO: QTimer::singleShot(500, static_cast<TreeItem*>(item), SLOT(reset()));
}

void Reseter::resetItems()
{
    QTreeWidgetItemIterator it(d.tree);
    while (*it) {
        // TODO: static_cast<QTreeWidgetItem*>(*it)->reset();
        ++it;
    }
}
