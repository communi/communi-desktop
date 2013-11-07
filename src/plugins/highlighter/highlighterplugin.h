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

#ifndef HIGHLIGHTERPLUGIN_H
#define HIGHLIGHTERPLUGIN_H

#include <QSet>
#include <QtPlugin>
#include <QTreeWidget>
#include "treewidgetplugin.h"

class IrcBuffer;
class IrcMessage;

class HighlighterPlugin : public QObject, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    HighlighterPlugin(QObject* parent = 0);

    void initialize(TreeWidget* tree);

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
    void onBufferChanged();
    void blinkItems();

private:
    struct Private {
        bool blink;
        TreeWidget* tree;
        QSet<QTreeWidgetItem*> items;
    } d;
};

#endif // HIGHLIGHTERPLUGIN_H
