/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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
#include "messageview.h"
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
    header()->resizeSection(1, 18);

    viewport()->setAttribute(Qt::WA_Hover);
    setItemDelegate(new SessionTreeDelegate(this));

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDragDropMode(InternalMove);

    d.dropParent = 0;
    d.menuFactory = 0;

    d.colors[Active] = palette().color(QPalette::WindowText);
    d.colors[Inactive] = palette().color(QPalette::Disabled, QPalette::Highlight);
    d.colors[Alert] = palette().color(QPalette::Highlight);
    d.colors[Highlight] = palette().color(QPalette::Highlight);
    d.alertColor = d.colors.value(Alert);

    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(onItemSelectionChanged()));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
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
    in >> d.state;

    for (int i = 0; i < topLevelItemCount(); ++i)
        topLevelItem(i)->sortChildren(0, Qt::AscendingOrder);
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

QColor SessionTreeWidget::currentAlertColor() const
{
    return d.alertColor;
}

SessionTreeItem* SessionTreeWidget::viewItem(MessageView* view) const
{
    return d.viewItems.value(view);
}

SessionTreeItem* SessionTreeWidget::sessionItem(Session* session) const
{
    return d.sessionItems.value(session);
}

void SessionTreeWidget::addView(MessageView* view)
{
    SessionTreeItem* item = 0;
    if (view->viewType() == MessageView::ServerView) {
        item = new SessionTreeItem(view, this);
        Session* session = view->session();
        connect(session, SIGNAL(nameChanged(QString)), this, SLOT(updateSession()));
        connect(session, SIGNAL(networkChanged(QString)), this, SLOT(updateSession()));
        d.sessionItems.insert(session, item);
    } else {
        SessionTreeItem* parent = d.sessionItems.value(view->session());
        item = new SessionTreeItem(view, parent);
    }

    connect(view, SIGNAL(activeChanged()), this, SLOT(updateView()));
    connect(view, SIGNAL(receiverChanged(QString)), this, SLOT(updateView()));
    d.viewItems.insert(view, item);
    updateView(view);
}

void SessionTreeWidget::removeView(MessageView* view)
{
    if (view->viewType() == MessageView::ServerView)
        d.sessionItems.remove(view->session());
    delete d.viewItems.take(view);
}

void SessionTreeWidget::renameView(MessageView* view)
{
    SessionTreeItem* item = d.viewItems.value(view);
    if (item)
        item->setText(0, view->receiver());
}

void SessionTreeWidget::setCurrentView(MessageView* view)
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
            if (item->isAlerted() || item->isHighlighted()) {
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
            if (item->isAlerted() || item->isHighlighted()) {
                setCurrentItem(item);
                break;
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

void SessionTreeWidget::alert(SessionTreeItem* item)
{
    if (d.alertedItems.isEmpty())
        SharedTimer::instance()->registerReceiver(this, "alertTimeout");
    d.alertedItems.insert(item);
}

void SessionTreeWidget::unalert(SessionTreeItem* item)
{
    if (d.alertedItems.remove(item) && d.alertedItems.isEmpty())
        SharedTimer::instance()->unregisterReceiver(this, "alertTimeout");
}

void SessionTreeWidget::applySettings(const Settings& settings)
{
    QColor color(settings.colors.value(Settings::Highlight));
    setStatusColor(Alert, color);
    setStatusColor(Highlight, color);

    d.prevShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NavigateUp)));
    d.nextShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NavigateDown)));
    d.prevUnreadShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NextUnreadUp)));
    d.nextUnreadShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NextUnreadDown)));
    d.expandShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NavigateRight)));
    d.collapseShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::NavigateLeft)));
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

void SessionTreeWidget::updateView(MessageView* view)
{
    if (!view)
        view = qobject_cast<MessageView*>(sender());
    SessionTreeItem* item = d.viewItems.value(view);
    if (item) {
        if (!item->parent())
            item->setText(0, item->session()->name().isEmpty() ? item->session()->host() : item->session()->name());
        else
            item->setText(0, view->receiver());
        // re-read MessageView::isActive()
        item->emitDataChanged();
    }
}

void SessionTreeWidget::updateSession(Session* session)
{
    if (!session)
        session = qobject_cast<Session*>(sender());
    SessionTreeItem* item = d.sessionItems.value(session);
    if (item)
        item->setText(0, session->name().isEmpty() ? session->host() : session->name());
}

void SessionTreeWidget::onItemSelectionChanged()
{
    SessionTreeItem* item = static_cast<SessionTreeItem*>(selectedItems().value(0));
    if (item) {
        resetItem(item);
        emit currentViewChanged(item->session(), item->parent() ? item->text(0) : QString());
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

void SessionTreeWidget::onItemClicked(QTreeWidgetItem* item, int column)
{
    if (column == 1)
        emit closeItem(static_cast<SessionTreeItem*>(item));
}

void SessionTreeWidget::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);
    static_cast<SessionTreeItem*>(current)->setBadge(0);
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

void SessionTreeWidget::alertTimeout()
{
    bool active = d.alertColor == d.colors.value(Active);
    d.alertColor = d.colors.value(active ? Alert : Active);

    foreach (SessionTreeItem* item, d.alertedItems) {
        item->emitDataChanged();
        if (SessionTreeItem* p = static_cast<SessionTreeItem*>(item->parent()))
            if (!p->isExpanded())
                p->emitDataChanged();
    }
}

void SessionTreeWidget::resetItem(SessionTreeItem* item)
{
    unalert(item);
    item->setAlerted(false);
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
