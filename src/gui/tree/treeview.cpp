/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "treeview.h"
#include "treerole.h"
#include "treemodel.h"
#include "sharedtimer.h"
#include "treedelegate.h"
#include "treenavigator.h"
#include <IrcBufferModel>
#include <QHeaderView>
#include <IrcMessage>
#include <IrcBuffer>
#include <QShortcut>
#include <QTimer>
#include <QEvent>
#include <Irc>

TreeView::TreeView(QWidget* parent) : QTreeView(parent)
{
    setIndentation(0);
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setUniformRowHeights(true);
    setFocusPolicy(Qt::NoFocus);
    setSelectionBehavior(SelectRows);
    setItemDelegate(new TreeDelegate(this));
#ifdef Q_OS_MAC
    setVerticalScrollMode(ScrollPerPixel);
#endif

    d.currentBuffer = 0;
    d.itemResetBlocked = false;
    setModel(d.model = new TreeModel(this));
    connect(selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex)));

    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->resizeSection(1, fontMetrics().width("0000"));

#ifdef Q_OS_MAC
    const QString navigate("Ctrl+Alt+%1");
    const QString activate("Shift+Ctrl+Alt+%1");
#else
    const QString navigate("Alt+%1");
    const QString activate("Shift+Alt+%1");
#endif

    d.navigator = new TreeNavigator(this);

    QShortcut* shortcut = new QShortcut(QKeySequence(navigate.arg("Up")), this);
    connect(shortcut, SIGNAL(activated()), d.navigator, SLOT(moveToPrevItem()));

    shortcut = new QShortcut(QKeySequence(navigate.arg("Down")), this);
    connect(shortcut, SIGNAL(activated()), d.navigator, SLOT(moveToNextItem()));

    shortcut = new QShortcut(QKeySequence(activate.arg("Up")), this);
    connect(shortcut, SIGNAL(activated()), d.navigator, SLOT(moveToPrevActiveItem()));

    shortcut = new QShortcut(QKeySequence(activate.arg("Down")), this);
    connect(shortcut, SIGNAL(activated()), d.navigator, SLOT(moveToNextActiveItem()));

    shortcut = new QShortcut(QKeySequence("Ctrl+L"), this);
    connect(shortcut, SIGNAL(activated()), d.navigator, SLOT(moveToMostActiveItem()));

    shortcut = new QShortcut(QKeySequence(navigate.arg("Left")), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(collapseCurrent()));

    shortcut = new QShortcut(QKeySequence(navigate.arg("Right")), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(expandCurrent()));

    shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(resetAllItems()));
}

TreeView::~TreeView()
{
}

QSize TreeView::sizeHint() const
{
    return QSize(20 * fontMetrics().width('#'), QTreeView::sizeHint().height());
}

IrcBuffer* TreeView::currentBuffer() const
{
    return d.currentBuffer;
}

void TreeView::setCurrentBuffer(IrcBuffer* buffer)
{
    QModelIndex index = d.model->index(buffer);
    if (index.isValid())
        setCurrentIndex(index);
}

void TreeView::addModel(IrcBufferModel* model)
{
    d.model->addModel(model);
    connect(model, SIGNAL(added(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
}

void TreeView::removeModel(IrcBufferModel* model)
{
    d.model->removeModel(model);
    disconnect(model, SIGNAL(added(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
}

QModelIndexList TreeView::highlightedItems() const
{
    QModelIndexList items;
    foreach (const QModelIndex& index, d.highlightedItems)
        items += index;
    return items;
}

void TreeView::searchItem(const QString& search)
{
    if (!search.isEmpty()) {
        const QModelIndex current = currentIndex();
        QModelIndexList items = d.model->match(current, Qt::DisplayRole, search, 1, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
        if (!items.isEmpty() && !items.contains(current))
            setCurrentIndex(items.first());
        emit searched(!items.isEmpty());
    }
}

void TreeView::searchAgain(const QString& search)
{
    const QModelIndex current = currentIndex();
    const QModelIndex start = current.sibling(current.row() + 1, 0);
    QModelIndexList items = d.model->match(start, Qt::DisplayRole, search, 1, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
    if (!items.isEmpty())
        setCurrentIndex(items.first());
    emit searched(!items.isEmpty());
}

void TreeView::blockItemReset()
{
    d.itemResetBlocked = true;
}

void TreeView::unblockItemReset()
{
    d.itemResetBlocked = false;
    delayedItemReset();
}

void TreeView::highlight(IrcBuffer* buffer)
{
    QModelIndex item = d.model->index(buffer);
    if (item.isValid()) {
        if (d.highlightedItems.isEmpty())
            SharedTimer::instance()->registerReceiver(this, "highlightTimeout");
        d.highlightedItems.insert(item);
        d.model->setData(item, true, HighlightRole);
    }
}

void TreeView::unhighlight(IrcBuffer* buffer)
{
    QModelIndex item = d.model->index(buffer);
    if (item.isValid()) {
        if (d.highlightedItems.remove(item) && d.highlightedItems.isEmpty())
            SharedTimer::instance()->unregisterReceiver(this, "highlightTimeout");
        d.model->setData(item, false, HighlightRole);
    }
}

bool TreeView::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
        delayedItemReset();
    return QTreeView::event(event);
}

void TreeView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    QTreeView::rowsAboutToBeRemoved(parent, start, end);

    for (int i = start; i < end; ++i)
        d.resetedItems.remove(parent.child(i, 0));
}

void TreeView::onBufferAdded(IrcBuffer* buffer)
{
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));

    QModelIndex index = d.model->index(buffer);
    if (index.isValid()) {
        QModelIndex parent = index.parent();
        if (parent.isValid() && !isExpanded(parent))
            expand(parent);
    }
}

void TreeView::onBufferRemoved(IrcBuffer* buffer)
{
    unhighlight(buffer);
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void TreeView::onMessageReceived(IrcMessage* message)
{
    // TODO: or invisible/inactive window
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());
        if (buffer && buffer != d.currentBuffer) {
            QModelIndex index = d.model->index(buffer);
            if (index.isValid()) {
                int count = d.model->data(index, BadgeRole).toInt();
                d.model->setData(index, ++count, BadgeRole);
            }
        }
    }
}

void TreeView::onCurrentChanged(const QModelIndex& index)
{
    IrcBuffer* buffer = index.data(Irc::BufferRole).value<IrcBuffer*>();
    if (d.currentBuffer != buffer) {
        IrcBuffer* prevBuffer = d.currentBuffer;
        d.currentBuffer = buffer;
        if (!d.itemResetBlocked) {
            if (prevBuffer)
                resetItem(d.model->index(prevBuffer));
            delayedItemReset();
        }
        emit currentBufferChanged(buffer);
    }
}

void TreeView::collapseCurrent()
{
    QModelIndex index = currentIndex();
    if (index.parent().isValid())
        index = index.parent();
    if (index.isValid()) {
        collapse(index);
        setCurrentIndex(index);
    }
}

void TreeView::expandCurrent()
{
    QModelIndex index = currentIndex();
    if (index.parent().isValid())
        index = index.parent();
    if (index.isValid())
        expand(index);
}

void TreeView::highlightTimeout()
{
    static bool highlighted = false;

    foreach (const QModelIndex& item, d.highlightedItems) {
        d.model->setData(item, highlighted, HighlightRole);
        // TODO: collapsed items
    }

    highlighted = !highlighted;
}


void TreeView::resetAllItems()
{
    const int topRows = d.model->rowCount();
    for (int i = 0; i < topRows; ++i) {
        const QModelIndex parent = d.model->index(i, 0);
        const int childRows = d.model->rowCount(parent);
        for (int j = 0; j < childRows; ++j)
            resetItem(d.model->index(j, 0, parent));
    }
}

void TreeView::delayedItemReset()
{
    QModelIndex current = currentIndex();
    if (current.isValid()) {
        d.resetedItems.insert(current);
        QTimer::singleShot(500, this, SLOT(delayedItemResetTimeout()));
    }
}

void TreeView::delayedItemResetTimeout()
{
    if (!d.resetedItems.isEmpty()) {
        foreach (const QModelIndex& item, d.resetedItems)
            resetItem(item);
        d.resetedItems.clear();
    }
}

void TreeView::resetItem(const QModelIndex& index)
{
    if (index.isValid()) {
        d.model->setData(index, 0, BadgeRole);
        if (d.highlightedItems.remove(index))
            d.model->setData(index, false, HighlightRole);
    }
}
