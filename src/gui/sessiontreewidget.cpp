/*
* Copyright (C) 2008-2013 Communi authors
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
#include "sessiontreedelegate.h"
#include "sessiontreeitem.h"
#include "sessionmodel.h"
#include "sessionitem.h"
#include "serveritem.h"
#include "menufactory.h"
#include "sharedtimer.h"
#include "session.h"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QTimer>

SessionTreeWidget::SessionTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    setAnimated(true);
    setColumnCount(2);
    setIndentation(0);
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setFrameStyle(QFrame::NoFrame);

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->resizeSection(1, 22);

    setItemDelegate(new SessionTreeDelegate(this));

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDragDropMode(InternalMove);

    d.dropParent = 0;
    d.menuFactory = 0;

    d.colors[Active] = palette().color(QPalette::WindowText);
    d.colors[Inactive] = palette().color(QPalette::Disabled, QPalette::Highlight);
    d.colors[Highlight] = palette().color(QPalette::Highlight);

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

    d.prevUnreadShortcut = new QShortcut(this);
    connect(d.prevUnreadShortcut, SIGNAL(activated()), this, SLOT(moveToPrevUnreadItem()));

    d.nextUnreadShortcut = new QShortcut(this);
    connect(d.nextUnreadShortcut, SIGNAL(activated()), this, SLOT(moveToNextUnreadItem()));

    d.expandShortcut = new QShortcut(this);
    connect(d.expandShortcut, SIGNAL(activated()), this, SLOT(expandCurrentSession()));

    d.collapseShortcut = new QShortcut(this);
    connect(d.collapseShortcut, SIGNAL(activated()), this, SLOT(collapseCurrentSession()));

    d.mostActiveShortcut = new QShortcut(this);
    connect(d.mostActiveShortcut, SIGNAL(activated()), this, SLOT(moveToMostActiveItem()));

    applySettings(d.settings);
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
        QTreeWidgetItem* parent = topLevelItem(i);
        QStringList receivers;
        for (int j = 0; j < parent->childCount(); ++j)
            receivers += parent->child(j)->text(0);
        hash.insert(parent->text(0), receivers);
    }
    out << hash;
    return state;
}

void SessionTreeWidget::restoreState(const QByteArray& state)
{
    QDataStream in(state);
    in >> d.sortOrders;
}

MenuFactory* SessionTreeWidget::menuFactory() const
{
    if (!d.menuFactory) {
        SessionTreeWidget* that = const_cast<SessionTreeWidget*>(this);
        that->d.menuFactory = new MenuFactory(that);
    }
    return d.menuFactory;
}

void SessionTreeWidget::setMenuFactory(MenuFactory* factory)
{
    if (d.menuFactory && d.menuFactory->parent() == this)
        delete d.menuFactory;
    d.menuFactory = factory;
}

QColor SessionTreeWidget::statusColor(SessionTreeWidget::ItemStatus status) const
{
    return d.colors.value(status);
}

void SessionTreeWidget::setStatusColor(SessionTreeWidget::ItemStatus status, const QColor& color)
{
    d.colors[status] = color;
}

SessionTreeItem* SessionTreeWidget::sessionItem(SessionModel* session) const
{
    return d.sessionItems.value(session);
}

void SessionTreeWidget::addView(SessionItem* view)
{
    SessionTreeItem* item = 0;
    if (qobject_cast<ServerItem*>(view)) {
        item = new SessionTreeItem(view, this);
        connect(view->model(), SIGNAL(itemAdded(SessionItem*)), this, SLOT(addView(SessionItem*)));
        connect(view->model(), SIGNAL(itemRemoved(SessionItem*)), this, SLOT(removeView(SessionItem*)));
        connect(view->model(), SIGNAL(currentItemChanged(SessionItem*)), this, SLOT(setCurrentView(SessionItem*)));
        d.sessionItems.insert(view->model(), item);
    } else {
        SessionTreeItem* parent = d.sessionItems.value(view->model());
        item = new SessionTreeItem(view, parent);
        parent->sortChildren(0, Qt::AscendingOrder);
        expandItem(parent);
    }

    connect(view, SIGNAL(activeChanged(bool)), this, SLOT(updateView()));
    connect(view, SIGNAL(nameChanged(QString)), this, SLOT(updateView()));
    d.viewItems.insert(view, item);
    updateView(view);
}

void SessionTreeWidget::removeView(SessionItem* view)
{
    if (qobject_cast<ServerItem*>(view))
        d.sessionItems.remove(view->model());
    delete d.viewItems.take(view);
}

void SessionTreeWidget::renameView(SessionItem* view)
{
    SessionTreeItem* item = d.viewItems.value(view);
    if (item)
        item->setText(0, view->name());
}

void SessionTreeWidget::setCurrentView(SessionItem* view)
{
    SessionTreeItem* item = d.viewItems.value(view);
    if (item)
        setCurrentItem(item);
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

void SessionTreeWidget::moveToNextUnreadItem()
{
    QTreeWidgetItem* current = currentItem();
    if (current) {
        QTreeWidgetItemIterator it(current);
        while (*++it && *it != current) {
            SessionTreeItem* item = static_cast<SessionTreeItem*>(*it);
            if (item->badge() > 0) {
                setCurrentItem(item);
                break;
            }
        }
    }
}

void SessionTreeWidget::moveToPrevUnreadItem()
{
    QTreeWidgetItem* current = currentItem();
    if (current) {
        QTreeWidgetItemIterator it(current);
        while (*--it && *it != current) {
            SessionTreeItem* item = static_cast<SessionTreeItem*>(*it);
            if (item->badge() > 0) {
                setCurrentItem(item);
                break;
            }
        }
    }
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
        QList<QTreeWidgetItem*> items = findItems(search, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
        if (!items.isEmpty() && !items.contains(currentItem()))
            setCurrentItem(items.first());
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

void SessionTreeWidget::expandCurrentSession()
{
    QTreeWidgetItem* item = currentItem();
    if (item && item->parent())
        item = item->parent();
    if (item)
        expandItem(item);
}

void SessionTreeWidget::collapseCurrentSession()
{
    QTreeWidgetItem* item = currentItem();
    if (item && item->parent())
        item = item->parent();
    if (item) {
        collapseItem(item);
        setCurrentItem(item);
    }
}

void SessionTreeWidget::applySettings(const Settings& settings)
{
    QColor color(settings.colors.value(Settings::Highlight));
    setStatusColor(Highlight, color);

    d.prevShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::PreviousView)));
    d.nextShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NextView)));
    d.prevUnreadShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::PreviousUnreadView)));
    d.nextUnreadShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NextUnreadView)));
    d.expandShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::ExpandView)));
    d.collapseShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::CollapseView)));
    d.mostActiveShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::MostActiveView)));
    d.settings = settings;
}

void SessionTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    SessionTreeItem* item = static_cast<SessionTreeItem*>(itemAt(event->pos()));
    if (item) {
        QMenu* menu = menuFactory()->createSessionTreeMenu(item, this);
        menu->exec(event->globalPos());
        menu->deleteLater();
    }
}

void SessionTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if (!item || !item->parent() || item->parent() != d.dropParent)
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

void SessionTreeWidget::updateView(SessionItem* view)
{
    if (!view)
        view = qobject_cast<SessionItem*>(sender());
    SessionTreeItem* item = d.viewItems.value(view);
    if (item) {
        if (!item->parent()) {
            item->setText(0, item->session()->name().isEmpty() ? item->session()->host() : item->session()->name());
            item->d.sortOrder = d.sortOrders.value(item->text(0)).toStringList();
        } else {
            item->setText(0, view->name());
        }
        // re-read SessionItem::isActive()
        item->emitDataChanged();
    }
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
    if (previous)
        resetItem(static_cast<SessionTreeItem*>(previous));
    delayedItemReset();
    SessionTreeItem* item = static_cast<SessionTreeItem*>(current);
    if (item)
        emit currentViewChanged(item->session(), item->parent() ? item->text(0) : QString());
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

void SessionTreeWidget::resetItem(SessionTreeItem* item)
{
    item->setBadge(0);
    item->setHighlighted(false);
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
