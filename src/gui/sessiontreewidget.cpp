/*
* Copyright (C) 2008-2014 The Communi Project
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

#include "sessiontreewidget.h"
#include "sessiontreeitem.h"
#include "sessiontreemenu.h"
#include "styledscrollbar.h"
#include "settingsmodel.h"
#include "itemdelegate.h"
#include "application.h"
#include "sharedtimer.h"
#include "connection.h"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <IrcBuffer>
#include <QTimer>

SessionTreeWidget::SessionTreeWidget(QWidget* parent) : QTreeWidget(parent)
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

    setVerticalScrollBar(new StyledScrollBar(this));
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
    connect(Application::settings(), SIGNAL(changed()), this, SLOT(applySettings()));
}

QSize SessionTreeWidget::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeWidget::sizeHint().height());
}

QByteArray SessionTreeWidget::saveState() const
{
    QByteArray state;
    QDataStream out(&state, QIODevice::WriteOnly);

    QVariantHash hash;
    for (int i = 0; i < topLevelItemCount(); ++i) {
        SessionTreeItem* item = static_cast<SessionTreeItem*>(topLevelItem(i));
        if (item->currentSortOrder() == SessionTreeItem::Manual)
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

void SessionTreeWidget::restoreState(const QByteArray& state)
{
    QVariantHash hash;
    QDataStream in(state);
    in >> hash;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        SessionTreeItem* item = static_cast<SessionTreeItem*>(topLevelItem(i));
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

QColor SessionTreeWidget::statusColor(SessionTreeWidget::ItemStatus status) const
{
    return d.colors.value(status);
}

void SessionTreeWidget::setStatusColor(SessionTreeWidget::ItemStatus status, const QColor& color)
{
    d.colors[status] = color;
}

QColor SessionTreeWidget::currentBadgeColor() const
{
    if (!d.highlightColor.isValid() || d.highlightColor != d.colors.value(Active))
        return d.colors.value(BadgeHighlight);
    return d.colors.value(Badge);
}

QColor SessionTreeWidget::currentHighlightColor() const
{
    if (!d.highlightColor.isValid())
        return d.colors.value(Highlight);
    return d.highlightColor;
}

IrcBuffer* SessionTreeWidget::currentBuffer() const
{
    SessionTreeItem* item = static_cast<SessionTreeItem*>(currentItem());
    if (item)
        return item->buffer();
    return 0;
}

void SessionTreeWidget::setCurrentBuffer(IrcBuffer* buffer)
{
    SessionTreeItem* item = d.bufferItems.value(buffer);
    if (item)
        setCurrentItem(item);
}

SessionTreeItem* SessionTreeWidget::bufferItem(IrcBuffer* buffer) const
{
    return d.bufferItems.value(buffer);
}

SessionTreeItem* SessionTreeWidget::connectionItem(IrcConnection* connection) const
{
    return d.connectionItems.value(connection);
}

bool SessionTreeWidget::hasRestoredCurrent() const
{
    return d.currentRestored;
}

ViewInfos SessionTreeWidget::viewInfos(IrcConnection* connection) const
{
    ViewInfos views;
    SessionTreeItem* item = d.connectionItems.value(connection);
    if (item) {
        for (int i = 0; i < item->childCount(); ++i) {
            SessionTreeItem* child = static_cast<SessionTreeItem*>(item->child(i));
            IrcBuffer* buf = child->buffer();
            ViewInfo view;
            view.type = buf->isSticky() ? ViewInfo::Server : buf->isChannel() ? ViewInfo::Channel : ViewInfo::Query;
            view.name = buf->title();
            view.active = buf->isActive();
            view.expanded = item->isExpanded();
            views += view;
        }
    }
    return views;
}

void SessionTreeWidget::addBuffer(IrcBuffer* buffer)
{
    SessionTreeItem* item = 0;
    if (buffer->isSticky()) {
        item = new SessionTreeItem(buffer, this);
        IrcConnection* connection = buffer->connection();
        connect(connection, SIGNAL(displayNameChanged(QString)), this, SLOT(updateConnection()));
        d.connectionItems.insert(connection, item);
        const bool sortViews = Application::settings()->value("ui.sortViews").toBool();
        item->sort(sortViews ? SessionTreeItem::Alphabetic : SessionTreeItem::Manual);
    } else {
        SessionTreeItem* parent = d.connectionItems.value(buffer->connection());
        item = new SessionTreeItem(buffer, parent);
        parent->sortChildren(0, Qt::AscendingOrder);
    }

    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(updateBuffer()));
    connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(updateBuffer()));
    d.bufferItems.insert(buffer, item);
    updateBuffer(buffer);
}

void SessionTreeWidget::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky())
        d.connectionItems.remove(buffer->connection());
    delete d.bufferItems.take(buffer);
}

void SessionTreeWidget::renameBuffer(IrcBuffer* buffer)
{
    SessionTreeItem* item = d.bufferItems.value(buffer);
    if (item)
        item->setText(0, buffer->title());
}

void SessionTreeWidget::moveToNextItem()
{
    QTreeWidgetItem* item = nextItem(currentItem());
    if (!item)
        item = topLevelItem(0);
    setCurrentItem(item);
}

void SessionTreeWidget::moveToPrevItem()
{
    QTreeWidgetItem* item = previousItem(currentItem());
    if (!item)
        item = lastItem();
    setCurrentItem(item);
}

void SessionTreeWidget::moveToNextActiveItem()
{
    QTreeWidgetItem* item = findNextItem(currentItem(), 0, ItemDelegate::HighlightRole);
    if (!item)
        item = findNextItem(currentItem(), 1, ItemDelegate::BadgeRole);
    if (item)
        setCurrentItem(item);
}

void SessionTreeWidget::moveToPrevActiveItem()
{
    QTreeWidgetItem* item = findPrevItem(currentItem(), 0, ItemDelegate::HighlightRole);
    if (!item)
        item = findPrevItem(currentItem(), 1, ItemDelegate::BadgeRole);
    if (item)
        setCurrentItem(item);
}

void SessionTreeWidget::moveToMostActiveItem()
{
    SessionTreeItem* mostActive = 0;
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Unselected);
    while (*it) {
        SessionTreeItem* item = static_cast<SessionTreeItem*>(*it);

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

void SessionTreeWidget::search(const QString& search)
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

void SessionTreeWidget::searchAgain(const QString& search)
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

void SessionTreeWidget::blockItemReset()
{
    d.itemResetBlocked = true;
}

void SessionTreeWidget::unblockItemReset()
{
    d.itemResetBlocked = false;
    delayedItemReset();
}

void SessionTreeWidget::expandCurrentConnection()
{
    QTreeWidgetItem* item = currentItem();
    if (item && item->parent())
        item = item->parent();
    if (item)
        expandItem(item);
}

void SessionTreeWidget::collapseCurrentConnection()
{
    QTreeWidgetItem* item = currentItem();
    if (item && item->parent())
        item = item->parent();
    if (item) {
        collapseItem(item);
        setCurrentItem(item);
    }
}

void SessionTreeWidget::highlight(SessionTreeItem* item)
{
    if (d.highlightedItems.isEmpty())
        SharedTimer::instance()->registerReceiver(this, "highlightTimeout");
    d.highlightedItems.insert(item);
    item->setHighlighted(true);
}

void SessionTreeWidget::unhighlight(SessionTreeItem* item)
{
    if (d.highlightedItems.remove(item) && d.highlightedItems.isEmpty())
        SharedTimer::instance()->unregisterReceiver(this, "highlightTimeout");
    item->setHighlighted(false);
}

void SessionTreeWidget::applySettings()
{
    SettingsModel* settings = Application::settings();

    const bool sortViews = settings->value("ui.sortViews").toBool();

    setDragEnabled(!sortViews);
    setDropIndicatorShown(!sortViews);
    setDragDropMode(sortViews ? NoDragDrop : InternalMove);

    for (int i = 0; i < topLevelItemCount(); ++i) {
        SessionTreeItem* item = static_cast<SessionTreeItem*>(topLevelItem(i));
        item->sort(sortViews ? SessionTreeItem::Alphabetic : SessionTreeItem::Manual);
    }

    // TODO:
    bool dark = settings->value("ui.dark").toBool();
    d.colors[Active] = dark ? QColor("#dedede") : palette().color(QPalette::Text);
    d.colors[Inactive] = dark ? QColor("#666666") : palette().color(QPalette::Disabled, QPalette::Text);
    d.colors[Highlight] = QColor("#ff4040");
    d.colors[Badge] = dark ? QColor("#444444") : palette().color(QPalette::AlternateBase).darker(125);
    d.colors[BadgeHighlight] = dark ? QColor("#ff4040").darker(125) : QColor("#ff4040").lighter(125);

    static_cast<ItemDelegate*>(itemDelegate())->setDark(dark);

    d.nextShortcut->setKey(QKeySequence(settings->value("shortcuts.nextView").toString()));
    d.prevShortcut->setKey(QKeySequence(settings->value("shortcuts.previousView").toString()));
    d.nextActiveShortcut->setKey(QKeySequence(settings->value("shortcuts.nextActiveView").toString()));
    d.prevActiveShortcut->setKey(QKeySequence(settings->value("shortcuts.previousActiveView").toString()));
    d.expandShortcut->setKey(QKeySequence(settings->value("shortcuts.expandView").toString()));
    d.collapseShortcut->setKey(QKeySequence(settings->value("shortcuts.collapseView").toString()));
    d.mostActiveShortcut->setKey(QKeySequence(settings->value("shortcuts.mostActiveView").toString()));
    d.resetShortcut->setKey(QKeySequence(settings->value("shortcuts.resetViews").toString()));
}

void SessionTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    SessionTreeItem* item = static_cast<SessionTreeItem*>(itemAt(event->pos()));
    if (item) {
        SessionTreeMenu menu(this);
        menu.exec(item, event->globalPos());
    }
}

void SessionTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if (item && !d.dropParent)
        event->ignore();
    else if (item && d.dropParent && item->parent() != d.dropParent)
        event->ignore();
    else
        QTreeWidget::dragMoveEvent(event);
}

bool SessionTreeWidget::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
        delayedItemReset();
    return QTreeWidget::event(event);
}

QMimeData* SessionTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
    QTreeWidgetItem* item = items.value(0);
    d.dropParent = item ? item->parent() : 0;
    return QTreeWidget::mimeData(items);
}

void SessionTreeWidget::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    QTreeWidget::rowsAboutToBeRemoved(parent, start, end);
    SessionTreeItem* item = static_cast<SessionTreeItem*>(itemFromIndex(parent));
    if (item) {
        for (int i = start; i <= end; ++i) {
            SessionTreeItem* child = static_cast<SessionTreeItem*>(item->child(i));
            if (child) {
                item->d.highlightedChildren.remove(child);
                d.resetedItems.remove(child);
                unhighlight(child);
            }
        }
    }
}

void SessionTreeWidget::updateBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = qobject_cast<IrcBuffer*>(sender());
    SessionTreeItem* item = d.bufferItems.value(buffer);
    if (item) {
        if (!item->parent())
            item->setText(0, item->connection()->displayName());
        else
            item->setText(0, buffer->title());
        // re-read IrcBuffer::isActive()
        item->emitDataChanged();
    }
}

void SessionTreeWidget::updateConnection(IrcConnection* connection)
{
    if (!connection)
        connection = qobject_cast<Connection*>(sender());
    SessionTreeItem* item = d.connectionItems.value(connection);
    if (item)
        item->setText(0, connection->displayName());
}

void SessionTreeWidget::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<SessionTreeItem*>(item)->emitDataChanged();
}

void SessionTreeWidget::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<SessionTreeItem*>(item)->emitDataChanged();
}

void SessionTreeWidget::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.itemResetBlocked) {
        if (previous)
            resetItem(static_cast<SessionTreeItem*>(previous));
        delayedItemReset();
    }
    SessionTreeItem* item = static_cast<SessionTreeItem*>(current);
    if (item)
        emit currentBufferChanged(item->buffer());
}

void SessionTreeWidget::resetAllItems()
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        resetItem(static_cast<SessionTreeItem*>(*it));
        ++it;
    }
}

void SessionTreeWidget::delayedItemReset()
{
    SessionTreeItem* item = static_cast<SessionTreeItem*>(currentItem());
    if (item) {
        d.resetedItems.insert(item);
        QTimer::singleShot(500, this, SLOT(delayedItemResetTimeout()));
    }
}

void SessionTreeWidget::delayedItemResetTimeout()
{
    if (!d.resetedItems.isEmpty()) {
        foreach (SessionTreeItem* item, d.resetedItems)
            resetItem(item);
        d.resetedItems.clear();
    }
}

void SessionTreeWidget::highlightTimeout()
{
    bool active = d.highlightColor == d.colors.value(Active);
    d.highlightColor = d.colors.value(active ? Highlight : Active);

    foreach (SessionTreeItem* item, d.highlightedItems) {
        item->emitDataChanged();
        if (SessionTreeItem* p = static_cast<SessionTreeItem*>(item->parent()))
            if (!p->isExpanded())
                p->emitDataChanged();
    }
}

void SessionTreeWidget::resetItem(SessionTreeItem* item)
{
    item->setBadge(0);
    unhighlight(item);
}

QTreeWidgetItem* SessionTreeWidget::lastItem() const
{
    QTreeWidgetItem* item = topLevelItem(topLevelItemCount() - 1);
    if (item->childCount() > 0)
        item = item->child(item->childCount() - 1);
    return item;
}

QTreeWidgetItem* SessionTreeWidget::nextItem(QTreeWidgetItem* from) const
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

QTreeWidgetItem* SessionTreeWidget::previousItem(QTreeWidgetItem* from) const
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

QTreeWidgetItem* SessionTreeWidget::findNextItem(QTreeWidgetItem* from, int column, int role) const
{
    if (from) {
        QTreeWidgetItemIterator it(from);
        while (*++it && *it != from) {
            SessionTreeItem* item = static_cast<SessionTreeItem*>(*it);
            if (item->data(column, role).toBool())
                return item;
        }
    }
    return 0;
}

QTreeWidgetItem* SessionTreeWidget::findPrevItem(QTreeWidgetItem* from, int column, int role) const
{
    if (from) {
        QTreeWidgetItemIterator it(from);
        while (*--it && *it != from) {
            SessionTreeItem* item = static_cast<SessionTreeItem*>(*it);
            if (item->data(column, role).toBool())
                return item;
        }
    }
    return 0;
}
