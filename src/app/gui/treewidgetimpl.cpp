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

#include "treewidgetimpl.h"
#include "treeplugin.h"
#include "treedelegate.h"
#include <QPluginLoader> // TODO
#include <QHeaderView>

// TODO:
Q_IMPORT_PLUGIN(FinderPlugin)
Q_IMPORT_PLUGIN(MenuPlugin)
Q_IMPORT_PLUGIN(NavigatorPlugin)
Q_IMPORT_PLUGIN(HighlighterPlugin)

TreeWidgetImpl::TreeWidgetImpl(QWidget* parent) : TreeWidget(parent)
{
    setAnimated(true);
    setColumnCount(2);
    setIndentation(0);
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setUniformRowHeights(true);
    setFrameStyle(QFrame::NoFrame);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->resizeSection(1, fontMetrics().width("999"));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setItemDelegate(new TreeDelegate(this));

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreePlugin* plugin = qobject_cast<TreePlugin*>(instance);
        if (plugin)
            plugin->initialize(this);
    }
}

QSize TreeWidgetImpl::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeWidget::sizeHint().height());
}

QByteArray TreeWidgetImpl::saveState() const
{
    QByteArray state;
//    QDataStream out(&state, QIODevice::WriteOnly);

//    QVariantHash hash;
//    for (int i = 0; i < topLevelItemCount(); ++i) {
//        TreeItem* item = static_cast<TreeItem*>(topLevelItem(i));
//        if (item->currentSortOrder() == TreeItem::Manual)
//            item->resetManualSortOrder();
//        hash.insert(item->text(0), item->manualSortOrder());
//    }

//    if (QTreeWidgetItem* item = currentItem()) {
//        hash.insert("_currentText_", currentItem()->text(0));
//        QTreeWidgetItem* parent = item->parent();
//        if (!parent)
//            parent = invisibleRootItem();
//        hash.insert("_currentIndex_", parent->indexOfChild(item));
//        hash.insert("_currentParent_", parent->text(0));
//    }
//    out << hash;
    return state;
}

void TreeWidgetImpl::restoreState(const QByteArray& state)
{
//    QVariantHash hash;
//    QDataStream in(state);
//    in >> hash;

//    for (int i = 0; i < topLevelItemCount(); ++i) {
//        TreeItem* item = static_cast<TreeItem*>(topLevelItem(i));
//        QStringList order = hash.value(item->text(0)).toStringList();
//        if (order != item->manualSortOrder()) {
//            item->setManualSortOrder(order);
//            item->sortChildren(0, Qt::AscendingOrder);
//        }
//    }

//    if (!d.currentRestored && hash.contains("_currentText_")) {
//        QList<QTreeWidgetItem*> candidates = findItems(hash.value("_currentText_").toString(), Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive);
//        foreach (QTreeWidgetItem* candidate, candidates) {
//            QTreeWidgetItem* parent = candidate->parent();
//            if (!parent)
//                parent = invisibleRootItem();
//            if (parent->indexOfChild(candidate) == hash.value("_currentIndex_").toInt()
//                    && parent->text(0) == hash.value("_currentParent_").toString()) {
//                setCurrentItem(candidate);
//                d.currentRestored = true;
//                break;
//            }
//        }
//    }
}
