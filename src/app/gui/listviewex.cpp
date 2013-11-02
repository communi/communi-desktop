/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "listviewex.h"
#include "listplugin.h"
#include <QPluginLoader> // TODO
#include <QScrollBar>
#include <Irc>

ListViewEx::ListViewEx(QWidget* parent) : ListView(parent)
{
    setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ListPlugin* plugin = qobject_cast<ListPlugin*>(instance);
        if (plugin)
            plugin->initialize(this);
    }
}

QSize ListViewEx::sizeHint() const
{
    const int w = 16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width();
    return QSize(w, QListView::sizeHint().height());
}

void ListViewEx::contextMenuEvent(QContextMenuEvent* event)
{
    Q_UNUSED(event);
    // TODO
}

void ListViewEx::onDoubleClicked(const QModelIndex& index)
{
    if (index.isValid())
        emit doubleClicked(index.data(Irc::NameRole).toString());
}
