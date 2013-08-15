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

#ifndef SESSIONTREEWIDGET_H
#define SESSIONTREEWIDGET_H

#include <QTreeWidget>
#include <QShortcut>
#include <QColor>
#include <QHash>
#include "viewinfo.h"

class IrcConnection;
class MessageView;
class SessionTreeItem;

class SessionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    SessionTreeWidget(QWidget* parent = 0);

    QSize sizeHint() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

    enum ItemStatus { Active, Inactive, Highlight, Badge, BadgeHighlight };

    QColor statusColor(ItemStatus status) const;
    void setStatusColor(ItemStatus status, const QColor& color);

    QColor currentBadgeColor() const;
    QColor currentHighlightColor() const;

    SessionTreeItem* viewItem(MessageView* view) const;
    SessionTreeItem* connectionItem(IrcConnection* connection) const;

    bool hasRestoredCurrent() const;
    ViewInfos viewInfos(IrcConnection* connection) const;

public slots:
    void addView(MessageView* view);
    void removeView(MessageView* view);
    void renameView(MessageView* view);
    void setCurrentView(MessageView* view);

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

    void highlight(SessionTreeItem* item);
    void unhighlight(SessionTreeItem* item);

signals:
    void editConnection(IrcConnection* connection);
    void closeItem(SessionTreeItem* item);
    void currentViewChanged(IrcConnection* connection, const QString& view);
    void searched(bool result);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    bool event(QEvent* event);
    QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;
    void rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);

private slots:
    void updateView(MessageView* view = 0);
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
    void resetItem(SessionTreeItem* item);
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
        QSet<SessionTreeItem*> resetedItems;
        QSet<SessionTreeItem*> highlightedItems;
        QHash<MessageView*, SessionTreeItem*> viewItems;
        QHash<IrcConnection*, SessionTreeItem*> connectionItems;
        mutable QTreeWidgetItem* dropParent;
    } d;
    friend class SessionTreeItem;
};

#endif // SESSIONTREEWIDGET_H
