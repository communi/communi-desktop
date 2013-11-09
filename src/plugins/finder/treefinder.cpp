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

#include "treefinder.h"
#include "treewidget.h"

TreeFinder::TreeFinder(TreeWidget* tree) : Finder(tree)
{
    d.tree = tree;
}

void TreeFinder::find(const QString& text, bool forward, bool backward, bool typed)
{
    Q_UNUSED(backward);
    if (!d.tree || text.isEmpty())
        return;

    if (typed) {
        QList<QTreeWidgetItem*> items = d.tree->findItems(text, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if (items.isEmpty())
            items = d.tree->findItems(text, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
        if (!items.isEmpty() && !items.contains(d.tree->currentItem()))
            d.tree->setCurrentItem(items.first());
        setError(items.isEmpty());
    } else {
        QTreeWidgetItem* item = d.tree->currentItem();
        if (item) {
            QTreeWidgetItemIterator it(item);
            if (forward)
                ++it;
            else
                --it;
            bool wrapped = false;
            while (*it) {
                if ((*it)->text(0).contains(text, Qt::CaseInsensitive)) {
                    d.tree->setCurrentItem(*it);
                    return;
                }
                if (forward)
                    ++it;
                else
                    --it;
                if (!(*it) && !wrapped) {
                    if (forward)
                        it = QTreeWidgetItemIterator(d.tree);
                    else
                        it = QTreeWidgetItemIterator(lastItem());
                    wrapped = true;
                }
            }
        }
    }
}

void TreeFinder::relocate()
{
    QRect r = rect();
    QRect br = parentWidget()->rect();
    r.moveBottom(br.bottom());
    r.setWidth(br.width() + 2);
    r.translate(-1, -offset());
    setGeometry(r);
}

QTreeWidgetItem* TreeFinder::lastItem() const
{
    QTreeWidgetItem* item = d.tree->topLevelItem(d.tree->topLevelItemCount() - 1);
    if (item && item->childCount() > 0)
        item = item->child(item->childCount() - 1);
    return item;
}
