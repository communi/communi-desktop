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

#include <QTreeWidget>
#include <QShortcut>
#include <QColor>
#include <QHash>

class TreeItem;
class IrcBuffer;
class IrcConnection;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget* parent = 0);

    QSize sizeHint() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

    enum ItemStatus { Active, Inactive, Highlight, Badge, BadgeHighlight };

    QColor statusColor(ItemStatus status) const;
    void setStatusColor(ItemStatus status, const QColor& color);

    QColor currentBadgeColor() const;
    QColor currentHighlightColor() const;

    IrcBuffer* currentBuffer() const;
    TreeItem* bufferItem(IrcBuffer* buffer) const;
    TreeItem* connectionItem(IrcConnection* connection) const;

    bool hasRestoredCurrent() const;

public slots:
    void addBuffer(IrcBuffer* buffer);
    void removeBuffer(IrcBuffer* buffer);
    void renameBuffer(IrcBuffer* buffer);
    void setCurrentBuffer(IrcBuffer* buffer);

    void moveToNextItem();
    void moveToPrevItem();

    void moveToNextActiveItem();
    void moveToPrevActiveItem();

    void expandCurrentConnection();
    void collapseCurrentConnection();
    void moveToMostActiveItem();

    void search(const QString& search);
    void searchAgain(const QString& search);

    void blockItemReset();
    void unblockItemReset();

    void highlight(TreeItem* item);
    void unhighlight(TreeItem* item);

signals:
    void editConnection(IrcConnection* connection);
    void closeItem(TreeItem* item);
    void currentBufferChanged(IrcBuffer* buffer);
    void searched(bool result);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    bool event(QEvent* event);
    QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;
    void rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);

private slots:
    void updateBuffer(IrcBuffer* buffer = 0);
    void updateConnection(IrcConnection* connection = 0);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void resetAllItems();
    void delayedItemReset();
    void delayedItemResetTimeout();
    void highlightTimeout();
    void applySettings();

private:
    void resetItem(TreeItem* item);
    QTreeWidgetItem* lastItem() const;
    QTreeWidgetItem* nextItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* previousItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* findNextItem(QTreeWidgetItem* from, int column, int role) const;
    QTreeWidgetItem* findPrevItem(QTreeWidgetItem* from, int column, int role) const;

    struct Private {
        bool currentRestored;
        bool itemResetBlocked;
        QColor highlightColor;
        QShortcut* prevShortcut;
        QShortcut* nextShortcut;
        QShortcut* prevActiveShortcut;
        QShortcut* nextActiveShortcut;
        QShortcut* expandShortcut;
        QShortcut* collapseShortcut;
        QShortcut* mostActiveShortcut;
        QShortcut* resetShortcut;
        QHash<ItemStatus, QColor> colors;
        QSet<TreeItem*> resetedItems;
        QSet<TreeItem*> highlightedItems;
        QHash<IrcBuffer*, TreeItem*> bufferItems;
        QHash<IrcConnection*, TreeItem*> connectionItems;
        mutable QTreeWidgetItem* dropParent;
    } d;
    friend class TreeItem;
};

#endif // TREEWIDGET_H
