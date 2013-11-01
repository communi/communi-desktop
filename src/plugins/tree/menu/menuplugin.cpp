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

#include "menuplugin.h"
#include "treewidget.h"
#include "treeitem.h"
#include <QContextMenuEvent>
#include <IrcConnection>
#include <IrcCommand>
#include <IrcChannel>

MenuPlugin::MenuPlugin(QObject* parent) : QMenu()
{
    Q_UNUSED(parent);

    d.tree = 0;
    d.item = 0;
    d.disconnectAction = addAction(tr("Disconnect"));
    d.reconnectAction = addAction(tr("Reconnect"));
    addSeparator();
    d.editAction = addAction(tr("Edit"), this, SLOT(onEditTriggered()));
    d.whoisAction = addAction(tr("Whois"), this, SLOT(onWhoisTriggered()));
    d.joinAction = addAction(tr("Join"), this, SLOT(onJoinTriggered()));
    d.partAction = addAction(tr("Part"), this, SLOT(onPartTriggered()));
    addSeparator();
    d.closeAction = addAction(tr("Close"), this, SLOT(onCloseTriggered()), QKeySequence::Close);
}

void MenuPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
    tree->installEventFilter(this);
}

bool MenuPlugin::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
        QTreeWidgetItem* item = d.tree->itemAt(cme->pos());
        if (item)
            exec(static_cast<TreeItem*>(item), cme->globalPos());
        return true;
    }
    return false;
}

void MenuPlugin::exec(TreeItem* item, const QPoint& pos)
{
    setup(item);
    updateActions();
    QMenu::exec(pos);
}

void MenuPlugin::onEditTriggered()
{
    // TODO: QMetaObject::invokeMethod(parent(), "editConnection", Q_ARG(IrcConnection*, d.item->connection()));
}

void MenuPlugin::onWhoisTriggered()
{
    IrcCommand* command = IrcCommand::createWhois(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void MenuPlugin::onJoinTriggered()
{
    IrcCommand* command = IrcCommand::createJoin(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void MenuPlugin::onPartTriggered()
{
    IrcCommand* command = IrcCommand::createPart(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void MenuPlugin::onCloseTriggered()
{
    d.item->buffer()->deleteLater();
}

void MenuPlugin::updateActions()
{
    if (d.item) {
        const bool child = d.item->parentItem();
        const bool connected = d.item->connection()->isActive();
        const bool active = d.item->buffer()->isActive();
        const bool channel = d.item->buffer()->isChannel();
        d.disconnectAction->setVisible(connected);
        d.reconnectAction->setVisible(!connected);
        d.editAction->setVisible(!connected && !child);
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
}

void MenuPlugin::setup(TreeItem* item)
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
}
