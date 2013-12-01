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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QHash>
#include <QQueue>
#include <QPointer>
#include <QTreeWidget>
#include <QStringList>

class TreeItem;
class IrcBuffer;
class IrcMessage;
class IrcConnection;

typedef QHash<QString, QStringList> QHashStringList;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* currentBuffer READ currentBuffer WRITE setCurrentBuffer NOTIFY currentBufferChanged)

public:
    explicit TreeWidget(QWidget* parent = 0);

    IrcBuffer* currentBuffer() const;
    TreeItem* bufferItem(IrcBuffer* buffer) const;
    TreeItem* connectionItem(IrcConnection* connection) const;

    bool blockItemReset(bool block);

    bool isSortingBlocked() const;
    void setSortingBlocked(bool blocked);

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

public slots:
    void addBuffer(IrcBuffer* buffer);
    void removeBuffer(IrcBuffer* buffer);
    void setCurrentBuffer(IrcBuffer* buffer);
    void closeBuffer(IrcBuffer* buffer = 0);

    void moveToNextItem();
    void moveToPrevItem();

    void moveToNextActiveItem();
    void moveToPrevActiveItem();

    void expandCurrentConnection();
    void collapseCurrentConnection();
    void moveToMostActiveItem();

signals:
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);
    void currentItemChanged(TreeItem* item);
    void currentBufferChanged(IrcBuffer* buffer);
    void bufferClosed(IrcBuffer* buffer);

protected:
    QSize sizeHint() const;
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private slots:
    void resetBadge(QTreeWidgetItem* item = 0);
    void delayedResetBadge(QTreeWidgetItem* item);
    void onMessageReceived(IrcMessage* message);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onItemDestroyed(TreeItem* item);
    void blinkItems();
    void resetItems();

    void onEditTriggered();
    void onWhoisTriggered();
    void onJoinTriggered();
    void onPartTriggered();
    void onCloseTriggered();

private:
    void swapItems(QTreeWidgetItem* source, QTreeWidgetItem* target);

    void highlightItem(QTreeWidgetItem* item);
    void unhighlightItem(QTreeWidgetItem* item);
    void updateHighlight(QTreeWidgetItem* item);

    QTreeWidgetItem* lastItem() const;
    QTreeWidgetItem* nextItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* previousItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* findNextItem(QTreeWidgetItem* from, int column, int role) const;
    QTreeWidgetItem* findPrevItem(QTreeWidgetItem* from, int column, int role) const;

    void initSortOrder();
    void saveSortOrder();
    void restoreSortOrder();

    friend class TreeItem;
    bool lessThan(const TreeItem* one, const TreeItem* another) const;

    QMenu* createContextMenu(TreeItem* item);

    struct Private {
        bool block;
        bool blink;
        QVariantMap sorting;
        bool sortingBlocked;
        QTreeWidgetItem* source;
        QStringList parentOrder;
        QHashStringList childrenOrders;
        QList<IrcConnection*> connections;
        QQueue<QPointer<TreeItem> > resetBadges;
        QSet<QTreeWidgetItem*> highlightedItems;
        QHash<IrcBuffer*, TreeItem*> bufferItems;
        QHash<IrcConnection*, TreeItem*> connectionItems;
    } d;
};

#endif // TREEWIDGET_H
