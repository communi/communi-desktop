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
#include "sharedtimer.h"
#include <QContextMenuEvent>
#include <IrcBufferModel>
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

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->resizeSection(1, fontMetrics().width("999"));

    invisibleRootItem()->setFlags(invisibleRootItem()->flags() | Qt::ItemIsDropEnabled);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ItemDelegate* delegate = new ItemDelegate(this);
    delegate->setRootIsDecorated(true);
    setItemDelegate(delegate);

    d.dropParent = 0;
    d.currentRestored = false;
    d.itemResetBlocked = false;

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    d.prevShortcut = new QShortcut(this);
    connect(d.prevShortcut, SIGNAL(activated()), this, SLOT(moveToPrevItem()));

    d.nextShortcut = new QShortcut(this);
    connect(d.nextShortcut, SIGNAL(activated()), this, SLOT(moveToNextItem()));

    d.prevActiveShortcut = new QShortcut(this);
    connect(d.prevActiveShortcut, SIGNAL(activated()), this, SLOT(moveToPrevActiveItem()));

    d.nextActiveShortcut = new QShortcut(this);
    connect(d.nextActiveShortcut, SIGNAL(activated()), this, SLOT(moveToNextActiveItem()));

    d.expandShortcut = new QShortcut(this);
    connect(d.expandShortcut, SIGNAL(activated()), this, SLOT(expandCurrentConnection()));

    d.collapseShortcut = new QShortcut(this);
    connect(d.collapseShortcut, SIGNAL(activated()), this, SLOT(collapseCurrentConnection()));

    d.mostActiveShortcut = new QShortcut(this);
    connect(d.mostActiveShortcut, SIGNAL(activated()), this, SLOT(moveToMostActiveItem()));

    d.resetShortcut = new QShortcut(this);
    connect(d.resetShortcut, SIGNAL(activated()), this, SLOT(resetAllItems()));

    applySettings();
}

QSize TreeWidget::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeWidget::sizeHint().height());
}

QByteArray TreeWidget::saveState() const
{
    QByteArray state;
    QDataStream out(&state, QIODevice::WriteOnly);

    QVariantHash hash;
    for (int i = 0; i < topLevelItemCount(); ++i) {
        TreeItem* item = static_cast<TreeItem*>(topLevelItem(i));
        if (item->currentSortOrder() == TreeItem::Manual)
            item->resetManualSortOrder();
        hash.insert(item->text(0), item->manualSortOrder());
    }

    if (QTreeWidgetItem* item = currentItem()) {
        hash.insert("_currentText_", currentItem()->text(0));
        QTreeWidgetItem* parent = item->parent();
        if (!parent)
            parent = invisibleRootItem();
        hash.insert("_currentIndex_", parent->indexOfChild(item));
        hash.insert("_currentParent_", parent->text(0));
    }
    out << hash;
    return state;
}

void TreeWidget::restoreState(const QByteArray& state)
{
    QVariantHash hash;
    QDataStream in(state);
    in >> hash;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        TreeItem* item = static_cast<TreeItem*>(topLevelItem(i));
        QStringList order = hash.value(item->text(0)).toStringList();
        if (order != item->manualSortOrder()) {
            item->setManualSortOrder(order);
            item->sortChildren(0, Qt::AscendingOrder);
        }
    }

    if (!d.currentRestored && hash.contains("_currentText_")) {
        QList<QTreeWidgetItem*> candidates = findItems(hash.value("_currentText_").toString(), Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive);
        foreach (QTreeWidgetItem* candidate, candidates) {
            QTreeWidgetItem* parent = candidate->parent();
            if (!parent)
                parent = invisibleRootItem();
            if (parent->indexOfChild(candidate) == hash.value("_currentIndex_").toInt()
                    && parent->text(0) == hash.value("_currentParent_").toString()) {
                setCurrentItem(candidate);
                d.currentRestored = true;
                break;
            }
        }
    }
}

QColor TreeWidget::statusColor(TreeWidget::ItemStatus status) const
{
    return d.colors.value(status);
}

void TreeWidget::setStatusColor(TreeWidget::ItemStatus status, const QColor& color)
{
    d.colors[status] = color;
}

QColor TreeWidget::currentBadgeColor() const
{
    if (!d.highlightColor.isValid() || d.highlightColor != d.colors.value(Active))
        return d.colors.value(BadgeHighlight);
    return d.colors.value(Badge);
}

QColor TreeWidget::currentHighlightColor() const
{
    if (!d.highlightColor.isValid())
        return d.colors.value(Highlight);
    return d.highlightColor;
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

TreeItem* TreeWidget::connectionItem(IrcConnection* connection) const
{
    return d.connectionItems.value(connection);
}

bool TreeWidget::hasRestoredCurrent() const
{
    return d.currentRestored;
}

void TreeWidget::addBuffer(IrcBuffer* buffer)
{
    TreeItem* item = 0;
    if (buffer->isSticky()) {
        item = new TreeItem(buffer, this);
        IrcConnection* connection = buffer->connection();
        connect(connection, SIGNAL(displayNameChanged(QString)), this, SLOT(updateConnection()));
        d.connectionItems.insert(connection, item);
        // TODO: const bool sortViews = Application::settings()->value("ui.sortViews").toBool();
        // TODO: item->sort(sortViews ? TreeItem::Alphabetic : TreeItem::Manual);
    } else {
        TreeItem* parent = d.connectionItems.value(buffer->connection());
        item = new TreeItem(buffer, parent);
        parent->sortChildren(0, Qt::AscendingOrder);
    }

    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(updateBuffer()));
    connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(updateBuffer()));
    d.bufferItems.insert(buffer, item);
    updateBuffer(buffer);
}

void TreeWidget::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky())
        d.connectionItems.remove(buffer->connection());
    delete d.bufferItems.take(buffer);
}

void TreeWidget::renameBuffer(IrcBuffer* buffer)
{
    TreeItem* item = d.bufferItems.value(buffer);
    if (item)
        item->setText(0, buffer->title());
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

void TreeWidget::highlight(TreeItem* item)
{
    if (d.highlightedItems.isEmpty())
        SharedTimer::instance()->registerReceiver(this, "highlightTimeout");
    d.highlightedItems.insert(item);
    item->setHighlighted(true);
}

void TreeWidget::unhighlight(TreeItem* item)
{
    if (d.highlightedItems.remove(item) && d.highlightedItems.isEmpty())
        SharedTimer::instance()->unregisterReceiver(this, "highlightTimeout");
    item->setHighlighted(false);
}

void TreeWidget::applySettings()
{
    const bool sortViews = true; // TODO: settings->value("ui.sortViews").toBool();

    setDragEnabled(!sortViews);
    setDropIndicatorShown(!sortViews);
    setDragDropMode(sortViews ? NoDragDrop : InternalMove);

    for (int i = 0; i < topLevelItemCount(); ++i) {
        TreeItem* item = static_cast<TreeItem*>(topLevelItem(i));
        item->sort(sortViews ? TreeItem::Alphabetic : TreeItem::Manual);
    }

    // TODO:
    bool dark = false; // TODO: settings->value("ui.dark").toBool();
    d.colors[Active] = dark ? QColor("#dedede") : palette().color(QPalette::Text);
    d.colors[Inactive] = dark ? QColor("#666666") : palette().color(QPalette::Disabled, QPalette::Text);
    d.colors[Highlight] = QColor("#ff4040");
    d.colors[Badge] = dark ? QColor("#444444") : palette().color(QPalette::AlternateBase).darker(125);
    d.colors[BadgeHighlight] = dark ? QColor("#ff4040").darker(125) : QColor("#ff4040").lighter(125);

    static_cast<ItemDelegate*>(itemDelegate())->setDark(dark);

#ifdef Q_OS_MAC
    QString navigate("Ctrl+Alt+%1");
    QString nextActive("Shift+Ctrl+Alt+%1");
#else
    QString navigate("Alt+%1");
    QString nextActive("Shift+Alt+%1");
#endif

    QHash<QString, QString> settings;
    settings["shortcuts.previousView"] = navigate.arg("Up");
    settings["shortcuts.nextView"] = navigate.arg("Down");
    settings["shortcuts.collapseView"] = navigate.arg("Left");
    settings["shortcuts.expandView"] = navigate.arg("Right");
    settings["shortcuts.previousActiveView"] = nextActive.arg("Up");
    settings["shortcuts.nextActiveView"] = nextActive.arg("Down");
    settings["shortcuts.mostActiveView"] = "Ctrl+L";
    settings["shortcuts.searchView"] = "Ctrl+S";
    settings["shortcuts.resetViews"] = "Ctrl+R";

    d.nextShortcut->setKey(QKeySequence(settings.value("shortcuts.nextView")));
    d.prevShortcut->setKey(QKeySequence(settings.value("shortcuts.previousView")));
    d.nextActiveShortcut->setKey(QKeySequence(settings.value("shortcuts.nextActiveView")));
    d.prevActiveShortcut->setKey(QKeySequence(settings.value("shortcuts.previousActiveView")));
    d.expandShortcut->setKey(QKeySequence(settings.value("shortcuts.expandView")));
    d.collapseShortcut->setKey(QKeySequence(settings.value("shortcuts.collapseView")));
    d.mostActiveShortcut->setKey(QKeySequence(settings.value("shortcuts.mostActiveView")));
    d.resetShortcut->setKey(QKeySequence(settings.value("shortcuts.resetViews")));
}

void TreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    TreeItem* item = static_cast<TreeItem*>(itemAt(event->pos()));
    if (item) {
        TreeMenu menu(this);
        menu.exec(item, event->globalPos());
    }
}

void TreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if (item && !d.dropParent)
        event->ignore();
    else if (item && d.dropParent && item->parent() != d.dropParent)
        event->ignore();
    else
        QTreeWidget::dragMoveEvent(event);
}

bool TreeWidget::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
        delayedItemReset();
    return QTreeWidget::event(event);
}

QMimeData* TreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
    QTreeWidgetItem* item = items.value(0);
    d.dropParent = item ? item->parent() : 0;
    return QTreeWidget::mimeData(items);
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
                unhighlight(child);
            }
        }
    }
}

void TreeWidget::updateBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = qobject_cast<IrcBuffer*>(sender());
    TreeItem* item = d.bufferItems.value(buffer);
    if (item) {
        if (!item->parent())
            item->setText(0, item->connection()->displayName());
        else
            item->setText(0, buffer->title());
        // re-read IrcBuffer::isActive()
        item->emitDataChanged();
    }
}

void TreeWidget::updateConnection(IrcConnection* connection)
{
    if (!connection)
        connection = qobject_cast<IrcConnection*>(sender());
    TreeItem* item = d.connectionItems.value(connection);
    if (item)
        item->setText(0, connection->displayName());
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
            resetItem(static_cast<TreeItem*>(previous));
        delayedItemReset();
    }
    TreeItem* item = static_cast<TreeItem*>(current);
    if (item)
        emit currentBufferChanged(item->buffer());
}

void TreeWidget::resetAllItems()
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        resetItem(static_cast<TreeItem*>(*it));
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
            resetItem(item);
        d.resetedItems.clear();
    }
}

void TreeWidget::highlightTimeout()
{
    bool active = d.highlightColor == d.colors.value(Active);
    d.highlightColor = d.colors.value(active ? Highlight : Active);

    foreach (TreeItem* item, d.highlightedItems) {
        item->emitDataChanged();
        if (TreeItem* p = static_cast<TreeItem*>(item->parent()))
            if (!p->isExpanded())
                p->emitDataChanged();
    }
}

void TreeWidget::resetItem(TreeItem* item)
{
    item->setBadge(0);
    unhighlight(item);
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
