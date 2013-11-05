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

#include <QtPlugin>
#include <QShortcut>
#include <QTreeWidget>
#include "treeplugin.h"

class IrcBuffer;
class IrcMessage;

class HighlighterPlugin : public QObject, public TreePlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")
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
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void resetItem(QTreeWidgetItem* item);
    void resetItems();

private:
    struct Private {
        TreeWidget* tree;
        QShortcut* shortcut;
    } d;
};

#endif // HIGHLIGHTERPLUGIN_H
