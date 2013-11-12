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
#include "bufferview.h"
#include "splitview.h"
#include "treeitem.h"
#include "window.h"
#include <IrcConnection>
#include <IrcBuffer>
#include <QSettings>
#include <QBitArray>
#include <QDebug>

StatePlugin::StatePlugin(QObject* parent) : QObject(parent)
{
    d.index = -1;
}

void StatePlugin::initialize(Window* window)
{
    QSettings settings;
    settings.beginGroup("States");
    if (settings.contains("window"))
        window->restoreGeometry(settings.value("window").toByteArray());
    settings.endGroup();

    settings.beginGroup("Connections");
    foreach (const QVariant& state, settings.value("connections").toList()) {
        IrcConnection* connection = new IrcConnection(this);
        connection->restoreState(state.toByteArray());
        window->addConnection(connection);
    }
}

void StatePlugin::uninitialize(Window* window)
{
    QSettings settings;
    settings.beginGroup("States");
    settings.setValue("window", window->saveGeometry());
    settings.endGroup();

    settings.beginGroup("Connections");
    QVariantList states;
    foreach (IrcConnection* connection, window->connections())
        states += connection->saveState();
    settings.setValue("connections", states);
}

void StatePlugin::initialize(SplitView* view)
{
    QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States/splitter");
        if (settings.contains("main"))
            splitter->restoreState(settings.value("main").toByteArray());
    } else {
        qWarning() << "StatePlugin: cannot restore ChatPage splitter state";
    }
    d.view = view;
    connect(view, SIGNAL(viewAdded(BufferView*)), this, SLOT(restoreView(BufferView*)));
    connect(view, SIGNAL(viewRemoved(BufferView*)), this, SLOT(saveView(BufferView*)));
    connect(view, SIGNAL(currentViewChanged(BufferView*)), this, SLOT(saveView(BufferView*)));
}

void StatePlugin::uninitialize(SplitView* view)
{
    QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States/splitter");
        settings.setValue("main", splitter->saveState());
    } else {
        qWarning() << "StatePlugin: cannot save ChatPage splitter state";
    }
    if (view->currentView())
        saveView(view->currentView());
}

void StatePlugin::initialize(TreeWidget* tree)
{
    QSettings settings;
    settings.beginGroup("States/tree");
    if (settings.contains("expanded")) {
        QBitArray expanded = settings.value("expanded").toBitArray();
        if (expanded.count() == tree->topLevelItemCount()) {
            for (int i = 0; i < expanded.count(); ++i)
                tree->topLevelItem(i)->setExpanded(expanded.testBit(i));
        } else {
            qDebug("TODO: restore tree state");
        }
    }
    d.tree = tree;
    d.current = settings.value("current").toString();
    d.parent = settings.value("parent").toString();
    d.index = settings.value("index", -1).toInt();
    if (!d.current.isEmpty() && d.index != -1) {
        connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
        connect(tree, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(onBufferChanged(IrcBuffer*)));
    }
}

void StatePlugin::uninitialize(TreeWidget* tree)
{
    QSettings settings;
    settings.beginGroup("States/tree");
    QBitArray expanded(tree->topLevelItemCount());
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = tree->topLevelItem(i);
        expanded.setBit(i, item->isExpanded());
    }
    settings.setValue("expanded", expanded);
    TreeItem* current = static_cast<TreeItem*>(tree->currentItem());
    TreeItem* parent = current ? current->parentItem() : 0;
    settings.setValue("parent", parent ? parent->text(0) : QString());
    settings.setValue("current", current ? current->text(0) : QString());
    settings.setValue("index", tree->indexOfTopLevelItem(parent ? parent : current));
}

void StatePlugin::restoreView(BufferView* view)
{
    QSplitter* splitter = view->findChild<QSplitter*>();
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States/splitter");
        if (settings.contains("buffer"))
            splitter->restoreState(settings.value("buffer").toByteArray());
        connect(splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(onSplitterMoved()), Qt::UniqueConnection);
    } else {
        qWarning() << "StatePlugin: cannot restore BufferView splitter state";
    }
}

void StatePlugin::saveView(BufferView* view)
{
    QSplitter* splitter = view->findChild<QSplitter*>();
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States/splitter");
        settings.setValue("buffer", splitter->saveState());
    } else {
        qWarning() << "StatePlugin: cannot save BufferView splitter state";
    }
}

void StatePlugin::onBufferAdded(IrcBuffer* buffer)
{
    if (buffer->title() == d.current) {
        TreeItem* item = d.tree->bufferItem(buffer);
        if (item) {
            TreeItem* parent = item->parentItem();
            if ((!parent && d.parent.isEmpty()) || (parent && parent->text(0) == d.parent)) {
                if (d.index == d.tree->indexOfTopLevelItem(parent ? parent : item))
                    d.tree->setCurrentBuffer(buffer);
            }
        }
    }
}

void StatePlugin::onBufferChanged(IrcBuffer* buffer)
{
    disconnect(d.tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    disconnect(d.tree, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(onBufferChanged(IrcBuffer*)));
    if (d.view->currentBuffer() == buffer)
        restoreView(d.view->currentView());
}

void StatePlugin::onSplitterMoved()
{
    QSplitter* splitter = qobject_cast<QSplitter*>(sender());
    if (splitter) {
        BufferView* view = qobject_cast<BufferView*>(splitter->parentWidget());
        if (view)
            saveView(view);
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(StatePlugin)
#endif
