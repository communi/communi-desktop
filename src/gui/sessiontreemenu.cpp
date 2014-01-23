/*
* Copyright (C) 2008-2014 The Communi Project
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

#include "sessiontreemenu.h"
#include "sessiontreewidget.h"
#include "sessiontreeitem.h"
#include "messageview.h"
#include "connection.h"
#include <IrcCommand>
#include <IrcChannel>

SessionTreeMenu::SessionTreeMenu(SessionTreeWidget* parent) : QMenu(parent)
{
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

void SessionTreeMenu::exec(SessionTreeItem* item, const QPoint& pos)
{
    setup(item);
    updateActions();
    QMenu::exec(pos);
}

void SessionTreeMenu::onEditTriggered()
{
    QMetaObject::invokeMethod(parent(), "editConnection", Q_ARG(IrcConnection*, d.item->connection()));
}

void SessionTreeMenu::onWhoisTriggered()
{
    IrcCommand* command = IrcCommand::createWhois(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void SessionTreeMenu::onJoinTriggered()
{
    IrcCommand* command = IrcCommand::createJoin(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void SessionTreeMenu::onPartTriggered()
{
    IrcCommand* command = IrcCommand::createPart(d.item->text(0));
    d.item->connection()->sendCommand(command);
}

void SessionTreeMenu::onCloseTriggered()
{
    QMetaObject::invokeMethod(parent(), "closeItem", Q_ARG(SessionTreeItem*, d.item));
}

void SessionTreeMenu::updateActions()
{
    if (d.item) {
        const bool child = d.item->parent();
        const bool connected = d.item->connection()->isActive();
        const bool active = d.item->view()->isActive();
        const bool channel = d.item->view()->buffer()->isChannel();
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

void SessionTreeMenu::setup(SessionTreeItem* item)
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
