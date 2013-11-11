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

#include "stateplugin.h"
#include "treewidget.h"
#include "splitview.h"
#include <QSettings>
#include <QBitArray>
#include <QDebug>

StatePlugin::StatePlugin(QObject* parent) : QObject(parent)
{
}

void StatePlugin::initialize(QWidget* window)
{
    QSettings settings;
    settings.beginGroup("States");
    if (settings.contains("window"))
        window->restoreGeometry(settings.value("window").toByteArray());
}

void StatePlugin::uninitialize(QWidget* window)
{
    QSettings settings;
    settings.beginGroup("States");
    settings.setValue("window", window->saveGeometry());
}

void StatePlugin::initialize(SplitView* view)
{
    QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States");
        if (settings.contains("splitter"))
            splitter->restoreState(settings.value("splitter").toByteArray());
    } else {
        qWarning() << "StatePlugin: cannot restore ChatPage splitter state";
    }
}

void StatePlugin::uninitialize(SplitView* view)
{
    QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States");
        settings.setValue("splitter", splitter->saveState());
    } else {
        qWarning() << "StatePlugin: cannot save ChatPage splitter state";
    }
}

void StatePlugin::initialize(TreeWidget* tree)
{
    QSettings settings;
    settings.beginGroup("States");
    settings.beginGroup("tree");
    if (settings.contains("expanded")) {
        QBitArray expanded = settings.value("expanded").toBitArray();
        if (expanded.count() == tree->topLevelItemCount()) {
            for (int i = 0; i < expanded.count(); ++i)
                tree->topLevelItem(i)->setExpanded(expanded.testBit(i));
        } else {
            qDebug("TODO: restore tree state");
        }
    }
}

void StatePlugin::uninitialize(TreeWidget* tree)
{
    QSettings settings;
    settings.beginGroup("States");
    settings.beginGroup("tree");
    QBitArray expanded(tree->topLevelItemCount());
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = tree->topLevelItem(i);
        expanded.setBit(i, item->isExpanded());
    }
    settings.setValue("expanded", expanded);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(StatePlugin)
#endif
