/*
  Copyright (C) 2008-2016 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "treewidget.h"
#include "treedelegate.h"
#include "textdocument.h"
#include "sharedtimer.h"
#include "treeitem.h"
#include "treerole.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>
#include <IrcMessage>
#include <IrcCommand>
#include <IrcChannel>
#include <IrcBuffer>
#include <QShortcut>
#include <QBitArray>
#include <QToolTip>
#include <QAction>
#include <QStyle>
#include <QTimer>
#include <QMenu>

TreeWidget::TreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    d.block = false;
    d.blink = false;
    d.pressedItem = 0;
    d.sortingBlocked = false;

    qRegisterMetaType<TreeItem*>();

    setAnimated(true);
    setColumnCount(2);
    setIndentation(0);
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setFocusPolicy(Qt::NoFocus);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef Q_OS_MAC
    setVerticalScrollMode(ScrollPerPixel);
#endif

    setItemDelegate(new TreeDelegate(this));

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
#ifdef Q_OS_WIN
    header()->resizeSection(1, fontMetrics().width("9999"));
#else
    header()->resizeSection(1, fontMetrics().width("999"));
#endif

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

#ifdef Q_OS_MAC
    QString navigate(tr("Ctrl+Alt+%1"));
    QString nextActive(tr("Shift+Ctrl+Alt+%1"));
#else
    QString navigate(tr("Alt+%1"));
    QString nextActive(tr("Shift+Alt+%1"));
#endif

    QShortcut* shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(navigate.arg("Up")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToPrevItem()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(navigate.arg("Down")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToNextItem()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(nextActive.arg("Up")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToPrevActiveItem()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(nextActive.arg("Down")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToNextActiveItem()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(navigate.arg("Right")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(expandCurrentConnection()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(navigate.arg("Left")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(collapseCurrentConnection()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(tr("Ctrl+L")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToMostActiveItem()));

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(resetItems()));
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

TreeDelegate* TreeWidget::itemDelegate() const
{
    return static_cast<TreeDelegate*>(QTreeWidget::itemDelegate());
}

bool TreeWidget::blockItemReset(bool block)
{
    bool wasBlocked = d.block;
    if (d.block != block) {
        d.block = block;
        QTreeWidgetItem* current = currentItem();
        if (!block && current) {
            delayedResetBadge(current);
            unhighlightItem(current);
        }
    }
    return wasBlocked;
}

bool TreeWidget::isSortingBlocked() const
{
    return d.sortingBlocked;
}

void TreeWidget::setSortingBlocked(bool blocked)
{
    if (d.sortingBlocked != blocked) {
        d.sortingBlocked = blocked;
        setSortingEnabled(!blocked);
    }
}

QByteArray TreeWidget::saveState() const
{
    QVariantMap state;
    QBitArray expanded(topLevelItemCount());
    for (int i = 0; i < topLevelItemCount(); ++i)
        expanded.setBit(i, topLevelItem(i)->isExpanded());
    state.insert("expanded", expanded);
    state.insert("sorting", d.sorting);

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << state;
    return data;
}

void TreeWidget::restoreState(const QByteArray& data)
{
    QVariantMap state;
    QDataStream in(data);
    in >> state;

    if (state.contains("expanded")) {
        QBitArray expanded = state.value("expanded").toBitArray();
        if (expanded.count() == topLevelItemCount()) {
            for (int i = 0; i < expanded.count(); ++i)
                topLevelItem(i)->setExpanded(expanded.testBit(i));
        }
    }
    if (state.contains("sorting")) {
        d.sorting = state.value("sorting").toMap();
        restoreSortOrder();
    }
}

void TreeWidget::addBuffer(IrcBuffer* buffer)
{
    TreeItem* item = 0;
    if (buffer->isSticky()) {
        item = new TreeItem(buffer, this);
        item->setFirstColumnSpanned(true);
        item->setExpanded(true);
        IrcConnection* connection = buffer->connection();
        d.connectionItems.insert(connection, item);
        d.connections.append(connection);
    } else {
        TreeItem* parent = d.connectionItems.value(buffer->connection());
        item = new TreeItem(buffer, parent);
    }
    connect(item, SIGNAL(destroyed(TreeItem*)), this, SLOT(onItemDestroyed(TreeItem*)));
    d.bufferItems.insert(buffer, item);
    emit bufferAdded(buffer);
}

void TreeWidget::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky()) {
        IrcConnection* connection = buffer->connection();
        d.connectionItems.remove(connection);
        d.connections.removeOne(connection);
    }
    emit bufferRemoved(buffer);
    delete d.bufferItems.take(buffer);
}

void TreeWidget::setCurrentBuffer(IrcBuffer* buffer)
{
    TreeItem* item = d.bufferItems.value(buffer);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::closeBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = currentBuffer();
    if (buffer)
        emit bufferClosed(buffer);
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
    QTreeWidgetItem* item = findNextItem(currentItem(), 1, TreeRole::Badge);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::moveToPrevActiveItem()
{
    QTreeWidgetItem* item = findPrevItem(currentItem(), 1, TreeRole::Badge);
    if (item)
        setCurrentItem(item);
}

void TreeWidget::moveToMostActiveItem()
{
    QTreeWidgetItem* mostActive = 0;
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Unselected);
    while (*it) {
        QTreeWidgetItem* item = *it;

        if (d.highlightedItems.contains(item)) {
            // we found a channel hilight or PM to us
            setCurrentItem(item);
            return;
        }

        // as a backup, store the most active window with any sort of activity
        const int badge = item->data(1, TreeRole::Badge).toInt();
        if (badge > 0 && (!mostActive || mostActive->data(1, TreeRole::Badge).toInt() < badge))
            mostActive = item;

        it++;
    }

    if (mostActive)
        setCurrentItem(mostActive);
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

QSize TreeWidget::sizeHint() const
{
    const int w = 16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width();
    return QSize(w, QTreeWidget::sizeHint().height());
}

bool TreeWidget::viewportEvent(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent* he = static_cast<QHelpEvent*>(event);
        TreeItem* item = static_cast<TreeItem*>(itemAt(he->pos()));
        if (item && !item->parentItem() && !item->toolTip(0).isEmpty()) {
            QStyleOptionViewItem opt = viewOptions();
            opt.icon = item->icon(0);
            opt.rect = visualItemRect(item);
            opt.features |= QStyleOptionViewItem::HasDecoration;
            QRect rect = style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, this);
            if (rect.contains(he->pos())) {
#if QT_VERSION >= 0x050200
                QToolTip::showText(he->globalPos(), item->toolTip(0), this, rect, 1250);
#else
                QToolTip::showText(he->globalPos(), item->toolTip(0), this, rect);
#endif
            }
        }
        return true;
    }
    return QTreeWidget::viewportEvent(event);
}

void TreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    TreeItem* item = static_cast<TreeItem*>(itemAt(event->pos()));
    if (item) {
        QMenu* menu = createContextMenu(item);
        menu->exec(event->globalPos());
        delete menu;
    }
}

void TreeWidget::mousePressEvent(QMouseEvent* event)
{
    d.pressedTime.start();
    d.pressedPoint = event->pos();
    QTreeWidget::mousePressEvent(event);
}

void TreeWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!d.pressedItem) {
        int time = d.pressedTime.elapsed();
        int distance = QPoint(event->pos() - d.pressedPoint).manhattanLength();
        if (time >= QApplication::startDragTime() && distance >= QApplication::startDragDistance())
            d.pressedItem = itemAt(d.pressedPoint);
    }
    if (d.pressedItem) {
        QTreeWidgetItem* target = itemAt(event->pos());
        if (target && d.pressedItem != target) {
            QTreeWidgetItem* parent = target->parent();
            if (parent == d.pressedItem->parent()) {
                setSortingBlocked(true);
                swapItems(d.pressedItem, target);
            }
        }
    }
    QTreeWidget::mouseMoveEvent(event);
}

void TreeWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (d.pressedItem && isSortingBlocked()) {
        initSortOrder();
        saveSortOrder();
    }
    setSortingBlocked(false);
    d.pressedItem = 0;
    QTreeWidget::mouseReleaseEvent(event);
}

void TreeWidget::resetBadge(QTreeWidgetItem* item)
{
    if (!item && !d.resetBadges.isEmpty())
        item = d.resetBadges.dequeue();
    if (item)
        item->setData(1, TreeRole::Badge, 0);
}

void TreeWidget::delayedResetBadge(QTreeWidgetItem* item)
{
    d.resetBadges.enqueue(static_cast<TreeItem*>(item));
    QTimer::singleShot(500, this, SLOT(resetBadge()));
}

void TreeWidget::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeWidget::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeWidget::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!d.block) {
        if (previous) {
            resetBadge(previous);
            unhighlightItem(previous);
        }
        if (current) {
            delayedResetBadge(current);
            unhighlightItem(current);
        }
    }

    TreeItem* item = static_cast<TreeItem*>(current);
    emit currentItemChanged(item);
    emit currentBufferChanged(item ? item->buffer() : 0);
}

void TreeWidget::onItemDestroyed(TreeItem* item)
{
    d.resetBadges.removeOne(item);
    d.highlightedItems.remove(item);
    d.bufferItems.remove(item->buffer());
}

void TreeWidget::blinkItems()
{
    foreach (QTreeWidgetItem* item, d.highlightedItems)
        updateHighlight(item);
    d.blink = !d.blink;
}

void TreeWidget::resetItems()
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        resetBadge(*it);
        unhighlightItem(*it);
        ++it;
    }
}

void TreeWidget::onEditTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        TreeItem* item = action->data().value<TreeItem*>();
        QMetaObject::invokeMethod(window(), "editConnection", Q_ARG(IrcConnection*, item->connection()));
    }
}

void TreeWidget::onWhoisTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        TreeItem* item = action->data().value<TreeItem*>();
        IrcCommand* command = IrcCommand::createWhois(item->text(0));
        item->connection()->sendCommand(command);
    }
}

void TreeWidget::onJoinTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        TreeItem* item = action->data().value<TreeItem*>();
        IrcCommand* command = IrcCommand::createJoin(item->text(0));
        item->connection()->sendCommand(command);
    }
}

void TreeWidget::onPartTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        TreeItem* item = action->data().value<TreeItem*>();
        IrcChannel* channel = item->buffer()->toChannel();
        if (channel && channel->isActive())
            channel->part(qApp->property("description").toString());
    }
}

void TreeWidget::onCloseTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        TreeItem* item = action->data().value<TreeItem*>();
        onPartTriggered();
        item->buffer()->deleteLater();
    }
}

void TreeWidget::swapItems(QTreeWidgetItem* source, QTreeWidgetItem* target)
{
    QTreeWidgetItem* parent = source->parent();
    if (!parent)
        parent = invisibleRootItem();
    const bool se = source->isExpanded();
    const bool ss = source->isFirstColumnSpanned();
    const bool te = target->isExpanded();
    const bool ts = target->isFirstColumnSpanned();
    const int idx = parent->indexOfChild(target);
    parent->takeChild(parent->indexOfChild(source));
    parent->insertChild(idx, source);
    source->setExpanded(se);
    source->setFirstColumnSpanned(ss);
    target->setExpanded(te);
    target->setFirstColumnSpanned(ts);
}

void TreeWidget::highlightItem(QTreeWidgetItem* item)
{
    if (item && !d.highlightedItems.contains(item)) {
        if (d.highlightedItems.isEmpty())
            SharedTimer::instance()->registerReceiver(this, "blinkItems");
        d.highlightedItems.insert(item);
        updateHighlight(item);
    }
}

void TreeWidget::unhighlightItem(QTreeWidgetItem* item)
{
    if (item && d.highlightedItems.contains(item)) {
        d.highlightedItems.remove(item);
        if (d.highlightedItems.isEmpty())
            SharedTimer::instance()->unregisterReceiver(this, "blinkItems");
        updateHighlight(item);
    }
}

void TreeWidget::updateHighlight(QTreeWidgetItem* item)
{
    TreeItem* ti = static_cast<TreeItem*>(item);
    if (ti) {
        const bool hilite = d.blink && d.highlightedItems.contains(item);
        item->setData(0, TreeRole::Highlight, hilite);
        item->setData(1, TreeRole::Highlight, hilite);
        TreeItem* pi = ti->parentItem();
        if (pi)
            pi->setData(0, TreeRole::Highlight, hilite && !pi->isExpanded());
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
            QTreeWidgetItem* item = *it;
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
            QTreeWidgetItem* item = *it;
            if (item->data(column, role).toBool())
                return item;
        }
    }
    return 0;
}

// TODO
class FriendlyModel : public IrcBufferModel
{
    friend class TreeWidget;
};

bool TreeWidget::lessThan(const TreeItem* one, const TreeItem* another) const
{
    QStringList order;
    const TreeItem* parent = one->parentItem();
    if (!parent)
        order = d.parentOrder;
    else if (!isSortingBlocked())
        order = d.childrenOrders.value(parent->text(0));
    const int oidx = order.indexOf(one->text(0));
    const int aidx = order.indexOf(another->text(0));
    if (oidx == -1  || aidx == -1) {
        if (!one->parentItem()) {
            QList<IrcConnection*> connections = one->treeWidget()->d.connections;
            return connections.indexOf(one->connection()) < connections.indexOf(another->connection());
        }
        if (one->buffer()) {
            const FriendlyModel* model = static_cast<FriendlyModel*>(one->buffer()->model());
            return model->lessThan(one->buffer(), another->buffer(), model->sortMethod());
        }
        return one->QTreeWidgetItem::operator<(*another);
    }
    return oidx < aidx;
}

void TreeWidget::initSortOrder()
{
    d.parentOrder.clear();
    d.childrenOrders.clear();
    for (int i = 0; i < topLevelItemCount(); ++i) {
        QStringList lst;
        QTreeWidgetItem* parent = topLevelItem(i);
        for (int j = 0; j < parent->childCount(); ++j)
            lst += parent->child(j)->text(0);
        d.childrenOrders.insert(parent->text(0), lst);
        d.parentOrder += parent->text(0);
    }
}

void TreeWidget::saveSortOrder()
{
    QHash<QString, QVariant> variants;
    QHashIterator<QString, QStringList> it(d.childrenOrders);
    while (it.hasNext()) {
        it.next();
        variants.insert(it.key(), it.value());
    }
    d.sorting.insert("children", variants);
    d.sorting.insert("parents", d.parentOrder);
}

void TreeWidget::restoreSortOrder()
{
    d.childrenOrders.clear();
    QHashIterator<QString, QVariant> it(d.sorting.value("children").toHash());
    while (it.hasNext()) {
        it.next();
        d.childrenOrders.insert(it.key(), it.value().toStringList());
    }
    d.parentOrder = d.sorting.value("parents").toStringList();
}

QMenu* TreeWidget::createContextMenu(TreeItem* item)
{
    QMenu* menu = new QMenu(this);
    menu->addAction(item->text(0))->setEnabled(false);
    menu->addSeparator();

    connect(item, SIGNAL(destroyed(TreeItem*)), menu, SLOT(deleteLater()));

    const bool child = item->parentItem();
    const bool connected = item->connection()->isActive();
    const bool waiting = item->connection()->status() == IrcConnection::Waiting;
    const bool active = item->buffer()->isActive();
    const bool channel = item->buffer()->isChannel();

    if (!child) {
        QAction* editAction = menu->addAction(tr("Edit"), this, SLOT(onEditTriggered()));
        editAction->setData(QVariant::fromValue(item));
        menu->addSeparator();

        if (waiting) {
            QAction* stopAction = menu->addAction(tr("Stop"));
            connect(stopAction, SIGNAL(triggered()), item->connection(), SLOT(setDisabled()));
            connect(stopAction, SIGNAL(triggered()), item->connection(), SLOT(close()));
        } else if (connected) {
            QAction* disconnectAction = menu->addAction(tr("Disconnect"));
            connect(disconnectAction, SIGNAL(triggered()), item->connection(), SLOT(setDisabled()));
            connect(disconnectAction, SIGNAL(triggered()), item->connection(), SLOT(quit()));
        } else {
            QAction* reconnectAction = menu->addAction(tr("Reconnect"));
            connect(reconnectAction, SIGNAL(triggered()), item->connection(), SLOT(setEnabled()));
            connect(reconnectAction, SIGNAL(triggered()), item->connection(), SLOT(open()));
        }
    }

    if (connected && child) {
        QAction* action = 0;
        if (!channel)
            action = menu->addAction(tr("Whois"), this, SLOT(onWhoisTriggered()));
        else if (!active)
            action = menu->addAction(tr("Join"), this, SLOT(onJoinTriggered()));
        else
            action = menu->addAction(tr("Part"), this, SLOT(onPartTriggered()));
        action->setData(QVariant::fromValue(item));
    }

    QAction* closeAction = menu->addAction(tr("Close"), this, SLOT(onCloseTriggered()), QKeySequence::Close);
    closeAction->setShortcutContext(Qt::WidgetShortcut);
    closeAction->setData(QVariant::fromValue(item));

    return menu;
}
