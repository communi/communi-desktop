/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class IrcBuffer;
class TreeModel;
class IrcMessage;
class TreeNavigator;
class IrcBufferModel;

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    TreeView(QWidget* parent = 0);
    ~TreeView();

    QSize sizeHint() const;

    IrcBuffer* currentBuffer() const;

    void addModel(IrcBufferModel* model);
    void removeModel(IrcBufferModel* model);

    QModelIndexList highlightedItems() const;

public slots:
    void setCurrentBuffer(IrcBuffer* buffer);

    void searchItem(const QString& search);
    void searchAgain(const QString& search);

    void blockItemReset();
    void unblockItemReset();

    void highlight(IrcBuffer* buffer);
    void unhighlight(IrcBuffer* buffer);

signals:
    void searched(bool result);
    void currentBufferChanged(IrcBuffer* buffer);

protected:
    bool event(QEvent* event);

protected slots:
    void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

private slots:
    void onBufferAdded(IrcBuffer* buffer);
    void onBufferRemoved(IrcBuffer* buffer);
    void onMessageReceived(IrcMessage* message);
    void onCurrentChanged(const QModelIndex& index);
    void collapseCurrent();
    void expandCurrent();
    void highlightTimeout();
    void resetAllItems();
    void delayedItemReset();
    void delayedItemResetTimeout();

private:
    void resetItem(const QModelIndex& index);

    struct Private {
        TreeModel* model;
        bool itemResetBlocked;
        TreeNavigator* navigator;
        IrcBuffer* currentBuffer;
        QSet<QPersistentModelIndex> resetedItems;
        QSet<QPersistentModelIndex> highlightedItems;
    } d;
};

#endif // TREEVIEW_H
