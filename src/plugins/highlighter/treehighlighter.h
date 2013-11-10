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

#ifndef TREEHIGHLIGHTER_H
#define TREEHIGHLIGHTER_H

#include <QSet>
#include <QObject>
#include <QTreeWidget>

class TreeItem;
class IrcBuffer;
class IrcMessage;
class TreeWidget;

class TreeHighlighter : public QObject
{
    Q_OBJECT

public:
    TreeHighlighter(TreeWidget* tree);

    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void onBufferAdded(IrcBuffer* buffer);
    void onBufferRemoved(IrcBuffer* buffer);
    void onMessageReceived(IrcMessage* message);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onItemChanged(QTreeWidgetItem* item, int column);
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void highlightItem(QTreeWidgetItem* item);
    void unhighlightItem(QTreeWidgetItem* item);
    void colorizeItem(QTreeWidgetItem* item);
    void onItemDestroyed(TreeItem* item);
    void onBufferChanged();
    void blinkItems();

private:
    struct Private {
        bool blink;
        bool block;
        TreeWidget* tree;
        QSet<QTreeWidgetItem*> items;
    } d;
};

#endif // TREEHIGHLIGHTER_H
