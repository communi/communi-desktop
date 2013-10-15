/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef TREENAVIGATOR_H
#define TREENAVIGATOR_H

#include <QObject>
#include <QModelIndex>

class TreeView;

class TreeNavigator : public QObject
{
    Q_OBJECT

public:
    TreeNavigator(TreeView* parent);

public slots:
    void moveToNextItem();
    void moveToPrevItem();
    void moveToNextActiveItem();
    void moveToPrevActiveItem();
    void moveToMostActiveItem();

protected:
    QModelIndex findNextItem(const QModelIndex& from, int flags = 0) const;
    QModelIndex findPrevItem(const QModelIndex& from, int flags = 0) const;

    QModelIndex findNextActiveItem(const QModelIndex& from, int flags = 0) const;
    QModelIndex findPrevActiveItem(const QModelIndex& from, int flags = 0) const;

    QModelIndex findNextHighlightedItem(const QModelIndex& from, int flags = 0) const;
    QModelIndex findPrevHighlightedItem(const QModelIndex& from, int flags = 0) const;

private:
    struct Private {
        TreeView* tree;
    } d;
};

#endif // TREENAVIGATOR_H
