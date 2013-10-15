/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "treenavigator.h"
#include "treeview.h"
#include "treerole.h"

TreeNavigator::TreeNavigator(TreeView* parent) : QObject(parent)
{
    d.tree = parent;
}

void TreeNavigator::moveToNextItem()
{
    QModelIndex next = findNextItem(d.tree->currentIndex(), Qt::MatchRecursive | Qt::MatchWrap);
    if (next.isValid()) {
        const QModelIndex parent = next.parent();
        // do not step into a collapsed branch
        if (parent.isValid() && !d.tree->isExpanded(parent))
            next = findNextItem(parent, Qt::MatchWrap);
        if (next.isValid())
            d.tree->setCurrentIndex(next);
    }
}

void TreeNavigator::moveToPrevItem()
{
    QModelIndex prev = findPrevItem(d.tree->currentIndex(), Qt::MatchRecursive | Qt::MatchWrap);
    if (prev.isValid()) {
        const QModelIndex parent = prev.parent();
        // do not step into a collapsed branch
        if (parent.isValid() && !d.tree->isExpanded(parent))
            prev = parent;
        if (prev.isValid())
            d.tree->setCurrentIndex(prev);
    }
}

void TreeNavigator::moveToNextActiveItem()
{
    QModelIndex next = findNextHighlightedItem(d.tree->currentIndex(), Qt::MatchRecursive);
    if (!next.isValid())
        next = findNextActiveItem(d.tree->currentIndex(), Qt::MatchRecursive);
    if (next.isValid())
        d.tree->setCurrentIndex(next);
}

void TreeNavigator::moveToPrevActiveItem()
{
    QModelIndex prev = findPrevHighlightedItem(d.tree->currentIndex(), Qt::MatchRecursive);
    if (!prev.isValid())
        prev = findPrevActiveItem(d.tree->currentIndex(), Qt::MatchRecursive);
    if (prev.isValid())
        d.tree->setCurrentIndex(prev);
}

void TreeNavigator::moveToMostActiveItem()
{
    const QModelIndex current = d.tree->currentIndex();

    int maxBadge = -1;
    int maxHighlight = -1;
    QModelIndex mostActive;
    QModelIndex mostHighlighted;
    QModelIndexList highlightedItems = d.tree->highlightedItems();
    QModelIndex item = findNextItem(current, Qt::MatchRecursive | Qt::MatchWrap);

    while (item.isValid() && item != current) {

        const int count = item.data(BadgeRole).toInt();
        if (count > 0) {
            if (highlightedItems.contains(item)) {
                // we found a channel highlight or PM to us
                if (maxHighlight == -1 || count > maxHighlight) {
                    mostHighlighted = item;
                    maxHighlight = count;
                }
            } else {
                // as a backup, store the most active window with any sort of activity
                if (maxBadge == -1 || count > maxBadge) {
                    mostActive = item;
                    maxBadge = count;
                }
            }
        }

        item = findNextItem(item, Qt::MatchRecursive | Qt::MatchWrap);
    }

    if (mostHighlighted.isValid())
        d.tree->setCurrentIndex(mostHighlighted);
    else if (mostActive.isValid())
        d.tree->setCurrentIndex(mostActive);
}

QModelIndex TreeNavigator::findNextItem(const QModelIndex& from, int flags) const
{
    if (!from.isValid())
        return QModelIndex();

    QModelIndex next;
    QModelIndex parent = from.parent();
    const QAbstractItemModel* model = from.model();
    const bool recurse = flags & Qt::MatchRecursive;
    const bool wrap = flags & Qt::MatchWrap;

    if (parent.isValid()) {
        // next sibling, fallback to next parent
        next = from.sibling(from.row() + 1, from.column());
        if (!next.isValid())
            next = parent.sibling(parent.row() + 1, parent.column());
    } else {
        if (recurse) {
            // this parent's first child
            next = from.child(0, from.column());
        } else {
            // next parent
            next = from.sibling(from.row() + 1, from.column());
        }
    }

    // wrap to first parent
    if (wrap && !next.isValid())
        next = model->index(0, from.column());

    return next;
}

QModelIndex TreeNavigator::findPrevItem(const QModelIndex& from, int flags) const
{
    if (!from.isValid())
        return QModelIndex();

    QModelIndex prev;
    QModelIndex parent = from.parent();
    const QAbstractItemModel* model = from.model();
    const bool recurse = flags & Qt::MatchRecursive;
    const bool wrap = flags & Qt::MatchWrap;

    if (parent.isValid()) {
        // previous sibling, fallback to own parent
        prev = from.sibling(from.row() - 1, from.column());
        if (!prev.isValid())
            prev = parent;
    } else {
        // previous parent
        prev = from.sibling(from.row() - 1, from.column());
        if (recurse) // last child
            prev = prev.child(model->rowCount(prev) - 1, from.column());
    }

    // wrap to last parent
    if (wrap && !prev.isValid()) {
        prev = model->index(model->rowCount() - 1, from.column());
        if (recurse) // last child
            prev = prev.child(model->rowCount(prev) - 1, from.column());
    }

    return prev;
}

QModelIndex TreeNavigator::findNextActiveItem(const QModelIndex& from, int flags) const
{
    QModelIndex next = findNextItem(from, flags);
    while (next.isValid()) {
        if (next.data(BadgeRole).toInt() > 0)
            return next;
        next = findNextItem(next, flags);
    }
    return QModelIndex();
}

QModelIndex TreeNavigator::findPrevActiveItem(const QModelIndex& from, int flags) const
{
    QModelIndex prev = findPrevItem(from, flags);
    while (prev.isValid()) {
        if (prev.data(BadgeRole).toInt() > 0)
            return prev;
        prev = findPrevItem(prev, flags);
    }
    return QModelIndex();
}

QModelIndex TreeNavigator::findNextHighlightedItem(const QModelIndex& from, int flags) const
{
    const QModelIndexList highlightedItems = d.tree->highlightedItems();
    QModelIndex next = findNextItem(from, flags);
    while (next.isValid()) {
        if (highlightedItems.contains(next))
            return next;
        next = findNextItem(next, flags);
    }
    return QModelIndex();
}

QModelIndex TreeNavigator::findPrevHighlightedItem(const QModelIndex& from, int flags) const
{
    const QModelIndexList highlightedItems = d.tree->highlightedItems();
    QModelIndex prev = findPrevItem(from, flags);
    while (prev.isValid()) {
        if (highlightedItems.contains(prev))
            return prev;
        prev = findPrevItem(prev, flags);
    }
    return QModelIndex();
}
