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

#include "reseterplugin.h"
#include "treewidget.h"
#include "treerole.h"

ReseterPlugin::ReseterPlugin(QObject* parent) : QObject(parent)
{
    d.tree = 0;
    d.shortcut = 0;
}

void ReseterPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;

    d.shortcut = new QShortcut(tree);
    d.shortcut->setKey(QKeySequence(tr("Ctrl+R")));
    connect(d.shortcut, SIGNAL(activated()), this, SLOT(resetItems()));
}

void ReseterPlugin::resetItems()
{
    QTreeWidgetItemIterator it(d.tree);
    while (*it) {
        (*it)->setData(0, TreeRole::Highlight, false);
        (*it)->setData(1, TreeRole::Badge, 0);
        ++it;
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(ReseterPlugin)
#endif
