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

#include "treewidget.h"
#include "treeitem.h"
#include "treemenu.h"
#include "itemdelegate.h"
#include <QContextMenuEvent>
#include <IrcConnection>
#include <QHeaderView>
#include <IrcBuffer>
#include <QTimer>

TreeWidget::TreeWidget(QWidget* parent) : QTreeWidget(parent)
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

    ItemDelegate* delegate = new ItemDelegate(this);
    delegate->setRootIsDecorated(true);
    setItemDelegate(delegate);

    d.itemResetBlocked = false;

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

#ifdef Q_OS_MAC
    QString navigate("Ctrl+Alt+%1");
    QString nextActive("Shift+Ctrl+Alt+%1");
#else
    QString navigate("Alt+%1");
    QString nextActive("Shift+Alt+%1");
#endif

    d.prevShortcut = new QShortcut(this);
    d.prevShortcut->setKey(QKeySequence(navigate.arg("Up")));
    connect(d.prevShortcut, SIGNAL(activated()), this, SLOT(moveToPrevItem()));

    d.nextShortcut = new QShortcut(this);
    d.nextShortcut->setKey(QKeySequence(navigate.arg("Down")));
    connect(d.nextShortcut, SIGNAL(activated()), this, SLOT(moveToNextItem()));

    d.prevActiveShortcut = new QShortcut(this);
    d.prevActiveShortcut->setKey(QKeySequence(nextActive.arg("Up")));
    connect(d.prevActiveShortcut, SIGNAL(activated()), this, SLOT(moveToPrevActiveItem()));

    d.nextActiveShortcut = new QShortcut(this);
    d.nextActiveShortcut->setKey(QKeySequence(nextActive.arg("Down")));
    connect(d.nextActiveShortcut, SIGNAL(activated()), this, SLOT(moveToNextActiveItem()));

    d.expandShortcut = new QShortcut(this);
    d.expandShortcut->setKey(QKeySequence(navigate.arg("Right")));
    connect(d.expandShortcut, SIGNAL(activated()), this, SLOT(expandCurrentConnection()));

    d.collapseShortcut = new QShortcut(this);
    d.collapseShortcut->setKey(QKeySequence(navigate.arg("Left")));
    connect(d.collapseShortcut, SIGNAL(activated()), this, SLOT(collapseCurrentConnection()));

    d.mostActiveShortcut = new QShortcut(this);
    d.mostActiveShortcut->setKey(QKeySequence(tr("Ctrl+L")));
    connect(d.mostActiveShortcut, SIGNAL(activated()), this, SLOT(moveToMostActiveItem()));

    d.resetShortcut = new QShortcut(this);
    d.resetShortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(d.resetShortcut, SIGNAL(activated()), this, SLOT(resetAllItems()));
}

QSize TreeWidget::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeWidget::sizeHint().height());
}

QByteArray TreeWidget::saveState() const
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

void TreeWidget::restoreState(const QByteArray& state)
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

IrcBuffer* TreeWidget::currentBuffer() const
{
    TreeItem* item = static_cast<TreeItem*>(currentItem());
    if (item)
        return item->buffer();
    return 0;
}

TreeItem* TreeWidget::bufferItem(IrcBuffer* buffer) const
{
    return d.bufferItems.value(buffer);
}

QList<IrcConnection*> TreeWidget::connections() const
{
    return d.connections;
}

TreeItem* TreeWidget::connectionItem(IrcConnection* connection) const
{
    return d.connectionItems.value(connection);
}

void TreeWidget::addBuffer(IrcBuffer* buffer)
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

void TreeWidget::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky()) {
        IrcConnection* connection = buffer->connection();
        d.connectionItems.remove(connection);
        d.connections.removeOne(connection);
    }
    delete d.bufferItems.take(buffer);
}

void TreeWidget::setCurrentBuffer(IrcBuffer* buffer)
{
    TreeItem* item = d.bufferItems.value(buffer);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::moveToNextItem()
{
    QTreeWidgetItem* item = nextItem(currentItem());
    if (!item)
        item = topLevelItem(0);
    setCurrentItem(item);
}

void TreeWidget::moveToPrevItem()
{
    QTreeWidgetItem* item = previousItem(currentItem());
    if (!item)
        item = lastItem();
    setCurrentItem(item);
}

void TreeWidget::moveToNextActiveItem()
{
    QTreeWidgetItem* item = findNextItem(currentItem(), 0, ItemDelegate::HighlightRole);
    if (!item)
        item = findNextItem(currentItem(), 1, ItemDelegate::BadgeRole);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::moveToPrevActiveItem()
{
    QTreeWidgetItem* item = findPrevItem(currentItem(), 0, ItemDelegate::HighlightRole);
    if (!item)
        item = findPrevItem(currentItem(), 1, ItemDelegate::BadgeRole);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::moveToMostActiveItem()
{
    TreeItem* mostActive = 0;
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Unselected);
    while (*it) {
        TreeItem* item = static_cast<TreeItem*>(*it);

        if (item->isHighlighted()) {
            // we found a channel hilight or PM to us
            setCurrentItem(item);
            return;
        }

        // as a backup, store the most active window with any sort of activity
        if (item->badge() && (!mostActive || mostActive->badge() < item->badge()))
            mostActive = item;

        it++;
    }

    if (mostActive)
        setCurrentItem(mostActive);
}

void TreeWidget::search(const QString& search)
{
    if (!search.isEmpty()) {
        QList<QTreeWidgetItem*> items = findItems(search, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if (items.isEmpty())
            items = findItems(search, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
        if (!items.isEmpty() && !items.contains(currentItem()))
            setCurrentItem(items.first());
        emit searched(!items.isEmpty());
    }
}

void TreeWidget::searchAgain(const QString& search)
{
    QTreeWidgetItem* item = currentItem();
    if (item && !search.isEmpty()) {
        QTreeWidgetItemIterator it(item, QTreeWidgetItemIterator::Unselected);
        bool wrapped = false;
        while (*it) {
            if ((*it)->text(0).contains(search, Qt::CaseInsensitive)) {
                setCurrentItem(*it);
                return;
            }
            ++it;
            if (!(*it) && !wrapped) {
                it = QTreeWidgetItemIterator(this, QTreeWidgetItemIterator::Unselected);
                wrapped = true;
            }
        }
    }
}

void TreeWidget::blockItemReset()
{
    d.itemResetBlocked = true;
}

void TreeWidget::unblockItemReset()
{
    d.itemResetBlocked = false;
    delayedItemReset();
}

void TreeWidget::expandCurrentConnection()
{
    QTreeWidgetItem* item = currentItem();
    if (item && item->parent())
        item = item->parent();
    if (item)
        expandItem(item);
}

void TreeWidget::collapseCurrentConnection()
{
    QTreeWidgetItem* item = currentItem();
    if (item && item->parent())
        item = item->parent();
    if (item) {
        collapseItem(item);
        setCurrentItem(item);
    }
}

bool TreeWidget::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
        delayedItemReset();
    return QTreeWidget::event(event);
}

void TreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    TreeItem* item = static_cast<TreeItem*>(itemAt(event->pos()));
    if (item) {
        TreeMenu menu(this);
        menu.exec(item, event->globalPos());
    }
}

void TreeWidget::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    QTreeWidget::rowsAboutToBeRemoved(parent, start, end);
    TreeItem* item = static_cast<TreeItem*>(itemFromIndex(parent));
    if (item) {
        for (int i = start; i <= end; ++i) {
            TreeItem* child = static_cast<TreeItem*>(item->child(i));
            if (child) {
                item->d.highlightedChildren.remove(child);
                d.resetedItems.remove(child);
                child->reset();
            }
        }
    }
}

void TreeWidget::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->emitDataChanged();
}

void TreeWidget::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->emitDataChanged();
}

void TreeWidget::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.itemResetBlocked) {
        if (previous)
            static_cast<TreeItem*>(previous)->reset();
        delayedItemReset();
    }
    if (current)
        emit currentBufferChanged(static_cast<TreeItem*>(current)->buffer());
}

void TreeWidget::resetAllItems()
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        static_cast<TreeItem*>(*it)->reset();
        ++it;
    }
}

void TreeWidget::delayedItemReset()
{
    TreeItem* item = static_cast<TreeItem*>(currentItem());
    if (item) {
        d.resetedItems.insert(item);
        QTimer::singleShot(500, this, SLOT(delayedItemResetTimeout()));
    }
}

void TreeWidget::delayedItemResetTimeout()
{
    if (!d.resetedItems.isEmpty()) {
        foreach (TreeItem* item, d.resetedItems)
            item->reset();
        d.resetedItems.clear();
    }
}

QTreeWidgetItem* TreeWidget::lastItem() const
{
    QTreeWidgetItem* item = topLevelItem(topLevelItemCount() - 1);
    if (item->childCount() > 0)
        item = item->child(item->childCount() - 1);
    return item;
}

QTreeWidgetItem* TreeWidget::nextItem(QTreeWidgetItem* from) const
{
    if (!from)
        return 0;
    QTreeWidgetItemIterator it(from);
    while (*++it) {
        if (!(*it)->parent() || (*it)->parent()->isExpanded())
            break;
    }
    return *it;
}

QTreeWidgetItem* TreeWidget::previousItem(QTreeWidgetItem* from) const
{
    if (!from)
        return 0;
    QTreeWidgetItemIterator it(from);
    while (*--it) {
        if (!(*it)->parent() || (*it)->parent()->isExpanded())
            break;
    }
    return *it;
}

QTreeWidgetItem* TreeWidget::findNextItem(QTreeWidgetItem* from, int column, int role) const
{
    if (from) {
        QTreeWidgetItemIterator it(from);
        while (*++it && *it != from) {
            TreeItem* item = static_cast<TreeItem*>(*it);
            if (item->data(column, role).toBool())
                return item;
        }
    }
    return 0;
}

QTreeWidgetItem* TreeWidget::findPrevItem(QTreeWidgetItem* from, int column, int role) const
{
    if (from) {
        QTreeWidgetItemIterator it(from);
        while (*--it && *it != from) {
            TreeItem* item = static_cast<TreeItem*>(*it);
            if (item->data(column, role).toBool())
                return item;
        }
    }
    return 0;
}
