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

#include "treemenu.h"
#include "treewidget.h"
#include "treeitem.h"
#include <QContextMenuEvent>
#include <QCoreApplication>
#include <IrcConnection>
#include <IrcCommand>
#include <IrcChannel>
#include <QShortcut>

TreeMenu::TreeMenu(TreeWidget* tree) : QMenu(tree)
{
    d.item = 0;
    d.tree = tree;

    d.disconnectAction = new QAction(tr("Disconnect"), tree);
    d.reconnectAction = new QAction(tr("Reconnect"), tree);
    d.editAction = new QAction(tr("Edit"), tree);
    d.whoisAction = new QAction(tr("Whois"), tree);
    d.joinAction = new QAction(tr("Join"), tree);
    d.partAction = new QAction(tr("Part"), tree);
    d.closeAction = new QAction(tr("Close"), tree);
    d.closeAction->setShortcut(QKeySequence::Close);

    connect(d.editAction, SIGNAL(triggered()), this, SLOT(onEditTriggered()));
    connect(d.whoisAction, SIGNAL(triggered()), this, SLOT(onWhoisTriggered()));
    connect(d.joinAction, SIGNAL(triggered()), this, SLOT(onJoinTriggered()));
    connect(d.partAction, SIGNAL(triggered()), this, SLOT(onPartTriggered()));
    connect(d.closeAction, SIGNAL(triggered()), this, SLOT(onCloseTriggered()));

    tree->installEventFilter(this);
}

bool TreeMenu::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
        QTreeWidgetItem* item = d.tree->itemAt(cme->pos());
        if (item) {
            setup(static_cast<TreeItem*>(item));
            exec(cme->globalPos());
        }
        return true;
    }
    return false;
}

void TreeMenu::onEditTriggered()
{
    QMetaObject::invokeMethod(d.tree->window(), "editConnection", Q_ARG(IrcConnection*, d.item->connection()));
}

void TreeMenu::onWhoisTriggered()
{
    IrcCommand* command = IrcCommand::createWhois(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void TreeMenu::onJoinTriggered()
{
    IrcCommand* command = IrcCommand::createJoin(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void TreeMenu::onPartTriggered()
{
    IrcChannel* channel = d.item->buffer()->toChannel();
    if (channel && channel->isActive())
        channel->part(qApp->property("description").toString());
}

void TreeMenu::onCloseTriggered()
{
    onPartTriggered();
    d.item->buffer()->deleteLater();
}

void TreeMenu::updateActions()
{
    if (d.item) {
        const bool child = d.item->parentItem();
        const bool connected = d.item->connection()->isActive();
        const bool active = d.item->buffer()->isActive();
        const bool channel = d.item->buffer()->isChannel();
        d.disconnectAction->setVisible(connected);
        d.reconnectAction->setVisible(!connected);
        d.editAction->setEnabled(!connected && !child);
        d.editAction->setVisible(!child);
        d.joinAction->setVisible(connected && channel && !active);
        d.partAction->setVisible(connected && channel && active);
        d.whoisAction->setVisible(connected && child && !channel);
    } else {
        d.disconnectAction->setVisible(false);
        d.reconnectAction->setVisible(false);
        d.editAction->setVisible(false);
        d.whoisAction->setVisible(false);
        d.joinAction->setVisible(false);
        d.partAction->setVisible(false);
    }

    d.closeAction->setVisible(d.item);
    d.closeAction->setEnabled(d.item);
}

void TreeMenu::setup(TreeItem* item)
{
    if (d.item != item) {
        if (d.item && d.item->connection()) {
            IrcConnection* connection = d.item->connection();
            disconnect(connection, SIGNAL(connecting()), this, SLOT(updateActions()));
            disconnect(connection, SIGNAL(connected()), this, SLOT(updateActions()));
            disconnect(connection, SIGNAL(disconnected()), this, SLOT(updateActions()));

            disconnect(d.disconnectAction, SIGNAL(triggered()), connection, SLOT(setDisabled()));
            disconnect(d.disconnectAction, SIGNAL(triggered()), connection, SLOT(quit()));

            disconnect(d.reconnectAction, SIGNAL(triggered()), connection, SLOT(setEnabled()));
            disconnect(d.reconnectAction, SIGNAL(triggered()), connection, SLOT(open()));
        }
        if (item && item->connection()) {
            IrcConnection* connection = item->connection();
            connect(connection, SIGNAL(connecting()), this, SLOT(updateActions()));
            connect(connection, SIGNAL(connected()), this, SLOT(updateActions()));
            connect(connection, SIGNAL(disconnected()), this, SLOT(updateActions()));

            connect(d.disconnectAction, SIGNAL(triggered()), connection, SLOT(setDisabled()));
            connect(d.disconnectAction, SIGNAL(triggered()), connection, SLOT(quit()));

            connect(d.reconnectAction, SIGNAL(triggered()), connection, SLOT(setEnabled()));
            connect(d.reconnectAction, SIGNAL(triggered()), connection, SLOT(open()));
        }
        d.item = item;
    }
    updateActions();

    clear();
    addAction(d.disconnectAction);
    addAction(d.reconnectAction);
    addSeparator();
    addAction(d.editAction);
    addAction(d.whoisAction);
    addAction(d.joinAction);
    addAction(d.partAction);
    if (!d.tree->actions().isEmpty()) {
        addSeparator();
        addActions(d.tree->actions());
    }
    addSeparator();
    addAction(d.closeAction);
}
