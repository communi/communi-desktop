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

#ifndef BADGEPLUGIN_H
#define BADGEPLUGIN_H

#include <QQueue>
#include <QtPlugin>
#include <QPointer>
#include <QTreeWidgetItem>
#include "treewidgetplugin.h"

class TreeItem;
class IrcBuffer;
class IrcMessage;

class BadgePlugin : public QObject, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    BadgePlugin(QObject* parent = 0);

    void initialize(TreeWidget* tree);

    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void onBufferAdded(IrcBuffer* buffer);
    void onBufferRemoved(IrcBuffer* buffer);
    void onMessageReceived(IrcMessage* message);
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

    void resetItem(QTreeWidgetItem* item = 0);
    void delayedResetItem(QTreeWidgetItem* item);

private:
    struct Private {
        bool block;
        TreeWidget* tree;
        QQueue<QPointer<TreeItem> > items;
    } d;
};

#endif // BADGEPLUGIN_H
