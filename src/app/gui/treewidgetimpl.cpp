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
#include "treeitem.h"
#include "treeplugin.h"
#include "treedelegate.h"
#include <IrcConnection>
#include <QPluginLoader> // TODO
#include <QHeaderView>
#include <IrcBuffer>

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

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem*)));
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

IrcBuffer* TreeWidgetImpl::currentBuffer() const
{
    TreeItem* item = static_cast<TreeItem*>(currentItem());
    if (item)
        return item->buffer();
    return 0;
}

TreeItem* TreeWidgetImpl::bufferItem(IrcBuffer* buffer) const
{
    return d.bufferItems.value(buffer);
}

QList<IrcConnection*> TreeWidgetImpl::connections() const
{
    return d.connections;
}

void TreeWidgetImpl::addBuffer(IrcBuffer* buffer)
{
    TreeItem* item = 0;
    if (buffer->isSticky()) {
        item = new TreeItem(buffer, this);
        IrcConnection* connection = buffer->connection();
        d.connectionItems.insert(connection, item);
        d.connections.append(connection);
    } else {
        TreeItem* parent = d.connectionItems.value(buffer->connection());
        item = new TreeItem(buffer, parent);
    }
    d.bufferItems.insert(buffer, item);
}

void TreeWidgetImpl::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky()) {
        IrcConnection* connection = buffer->connection();
        d.connectionItems.remove(connection);
        d.connections.removeOne(connection);
    }
    delete d.bufferItems.take(buffer);
}

void TreeWidgetImpl::setCurrentBuffer(IrcBuffer* buffer)
{
    TreeItem* item = d.bufferItems.value(buffer);
    if (item)
        setCurrentItem(item);
}

void TreeWidgetImpl::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeWidgetImpl::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeWidgetImpl::onCurrentItemChanged(QTreeWidgetItem* item)
{
    if (item)
        emit currentBufferChanged(static_cast<TreeItem*>(item)->buffer());
}
