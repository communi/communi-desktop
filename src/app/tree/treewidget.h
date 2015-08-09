/*
  Copyright (C) 2008-2015 The Communi Project

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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTime>
#include <QHash>
#include <QQueue>
#include <QPointer>
#include <QTreeWidget>
#include <QStringList>

class TreeItem;
class IrcBuffer;
class IrcMessage;
class IrcConnection;
class TreeDelegate;

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

    TreeDelegate* itemDelegate() const;

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

    void highlightItem(QTreeWidgetItem* item);
    void unhighlightItem(QTreeWidgetItem* item);

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
    bool viewportEvent(QEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private slots:
    void resetBadge(QTreeWidgetItem* item = 0);
    void delayedResetBadge(QTreeWidgetItem* item);
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
    void updateHighlight(QTreeWidgetItem* item);
    void swapItems(QTreeWidgetItem* source, QTreeWidgetItem* target);

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
        QTime pressedTime;
        QPoint pressedPoint;
        QStringList parentOrder;
        QTreeWidgetItem* pressedItem;
        QHashStringList childrenOrders;
        QList<IrcConnection*> connections;
        QQueue<QPointer<TreeItem> > resetBadges;
        QSet<QTreeWidgetItem*> highlightedItems;
        QHash<IrcBuffer*, TreeItem*> bufferItems;
        QHash<IrcConnection*, TreeItem*> connectionItems;
    } d;
};

#endif // TREEWIDGET_H
