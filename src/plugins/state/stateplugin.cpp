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
#include "mainwindow.h"
#include <IrcConnection>
#include <IrcBuffer>
#include <QSettings>
#include <QBitArray>
#include <QDebug>

StatePlugin::StatePlugin(QObject* parent) : QObject(parent)
{
    d.index = -1;
    d.tree = 0;
    d.view = 0;
}

void StatePlugin::initWindow(MainWindow* window)
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

void StatePlugin::cleanupWindow(MainWindow* window)
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

void StatePlugin::initView(SplitView* view)
{
    d.view = view;
    QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States/splitter");
        if (settings.contains("main"))
            splitter->restoreState(settings.value("main").toByteArray());
        if (settings.contains("views"))
            restoreSplittedViews(d.view, settings.value("views").toMap());
    } else {
        qWarning() << "StatePlugin: cannot restore ChatPage splitter state";
    }
    if (IrcBuffer* buffer = d.view->currentBuffer())
        restoreBuffer(buffer);
}

void StatePlugin::cleanupView(SplitView* view)
{
    QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
    if (splitter) {
        QSettings settings;
        settings.beginGroup("States/splitter");
        settings.setValue("main", splitter->saveState());
        settings.setValue("views", saveSplittedViews(d.view));
    } else {
        qWarning() << "StatePlugin: cannot save ChatPage splitter state";
    }
}

void StatePlugin::initTree(TreeWidget* tree)
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
    if (!d.current.isEmpty() && d.index != -1)
        connect(tree, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(resetCurrent()));
    connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(restoreBuffer(IrcBuffer*)));
}

void StatePlugin::cleanupTree(TreeWidget* tree)
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

//void StatePlugin::restoreView(BufferView* view)
//{
//    QSplitter* splitter = view->findChild<QSplitter*>();
//    if (splitter) {
//        QSettings settings;
//        settings.beginGroup("States/splitter");
//        if (settings.contains("buffer"))
//            splitter->restoreState(settings.value("buffer").toByteArray());
//        connect(splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(onSplitterMoved()), Qt::UniqueConnection);
//    } else {
//        qWarning() << "StatePlugin: cannot restore BufferView splitter state";
//    }
//}

//void StatePlugin::saveView(BufferView* view)
//{
//    QSplitter* splitter = view->findChild<QSplitter*>();
//    if (splitter) {
//        QSettings settings;
//        settings.beginGroup("States/splitter");
//        settings.setValue("buffer", splitter->saveState());
//    } else {
//        qWarning() << "StatePlugin: cannot save BufferView splitter state";
//    }
//}

void StatePlugin::resetCurrent()
{
    d.index = -1;
    d.parent.clear();
    d.current.clear();
    disconnect(d.tree, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(resetCurrent()));
}

void StatePlugin::restoreBuffer(IrcBuffer* buffer)
{
    if (!d.current.isEmpty() && buffer->title() == d.current) {
        TreeItem* item = d.tree->bufferItem(buffer);
        if (item) {
            TreeItem* parent = item->parentItem();
            if ((!parent && d.parent.isEmpty()) || (parent && parent->text(0) == d.parent)) {
                if (d.index == d.tree->indexOfTopLevelItem(parent ? parent : item)) {
                    d.tree->setCurrentBuffer(buffer);
                    d.current.clear();
                    d.parent.clear();
                    d.index = -1;
                }
            }
        }
    }

    // TODO: optimize
    QList<BufferView*> views = d.view->findChildren<BufferView*>("__unrestored__");
    foreach (BufferView* bv, views) {
        if (bv->property("__buffer__").toString() == buffer->title()) {
            TreeItem* item = d.tree->bufferItem(buffer);
            TreeItem* parent = item ? item->parentItem() : 0;
            if (bv->property("__parent__").toString() == (parent ? parent->text(0) : QString())) {
                if (bv->property("__index__").toInt() == d.tree->indexOfTopLevelItem(parent ? parent : item)) {
                    bv->setBuffer(buffer);
                    bv->setObjectName(QString());
                    bv->setProperty("__parent__", QVariant());
                    bv->setProperty("__buffer__", QVariant());
                    bv->setProperty("__index__", QVariant());
                }
            }
        }
    }

    if (d.current.isEmpty() && d.parent.isEmpty() && d.index == -1 && views.isEmpty())
        disconnect(d.tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(restoreBuffer(IrcBuffer*)));
}

QVariantMap StatePlugin::saveSplittedViews(QSplitter* splitter) const
{
    QVariantMap state;
    state.insert("count", splitter->count());
    if (QSplitter* parent = qobject_cast<QSplitter*>(splitter->parentWidget()))
        state.insert("index", parent->indexOf(splitter));
    state.insert("state", splitter->saveState());
    state.insert("geometry", splitter->saveGeometry());
    state.insert("orientation", splitter->orientation());
    QVariantList buffers;
    QVariantList children;
    for (int i = 0; i < splitter->count(); ++i) {
        QSplitter* child = qobject_cast<QSplitter*>(splitter->widget(i));
        if (child)
            children += saveSplittedViews(child);
        BufferView* bv = qobject_cast<BufferView*>(splitter->widget(i));
        if (bv) {
            TreeItem* item = d.tree->bufferItem(bv->buffer());
            TreeItem* parent = item ? item->parentItem() : 0;
            QVariantMap buffer;
            buffer.insert("parent", parent ? parent->text(0) : QString());
            buffer.insert("buffer", item ? item->text(0) : QString());
            buffer.insert("index", d.tree->indexOfTopLevelItem(parent ? parent : item));
            if (QSplitter* sp = bv->findChild<QSplitter*>())
                buffer.insert("state", sp->saveState());
            buffers += buffer;
        }
    }
    state.insert("buffers", buffers);
    state.insert("children", children);
    return state;
}

void StatePlugin::restoreSplittedViews(QSplitter* splitter, const QVariantMap& state, SplitView* view)
{
    if (!view)
        view = d.view;

    int count = state.value("count", -1).toInt();
    if (count > 1) {
        BufferView* bv = qobject_cast<BufferView*>(splitter->widget(0));
        Q_ASSERT(bv);
        Qt::Orientation orientation = static_cast<Qt::Orientation>(state.value("orientation").toInt());
        for (int i = 1; i < count; ++i)
            view->split(bv, orientation);

        QVariantList children = state.value("children").toList();
        foreach (const QVariant& v, children) {
            QVariantMap child = v.toMap();
            int index = child.value("index", -1).toInt();
            Qt::Orientation ori = static_cast<Qt::Orientation>(child.value("orientation").toInt());
            BufferView* bv = qobject_cast<BufferView*>(splitter->widget(index));
            if (bv) {
                QSplitter* parent = view->wrap(bv, ori);
                if (parent)
                    restoreSplittedViews(parent, child, view);
            }
        }
    }

    QVariantList buffers = state.value("buffers").toList();
    for (int i = 0; !buffers.isEmpty() && i < splitter->count(); ++i) {
        BufferView* bv = qobject_cast<BufferView*>(splitter->widget(i));
        if (bv) {
            QVariantMap buffer = buffers.takeFirst().toMap();
            bv->setProperty("__parent__", buffer.value("parent").toString());
            bv->setProperty("__buffer__", buffer.value("buffer").toString());
            bv->setProperty("__index__", buffer.value("index").toInt());
            if (buffer.contains("state")) {
                if (QSplitter* sp = bv->findChild<QSplitter*>())
                    sp->restoreState(buffer.value("state").toByteArray());
            }
            bv->setObjectName("__unrestored__");
        }
    }

    if (state.contains("geometry"))
        splitter->restoreGeometry(state.value("geometry").toByteArray());
    if (state.contains("state"))
        splitter->restoreState(state.value("state").toByteArray());
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(StatePlugin)
#endif
