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

#include "userlistmenu.h"
#include "userlistview.h"
#include "connection.h"
#include <IrcCommand>
#include <IrcChannel>

UserListMenu::UserListMenu(UserListView* view) : QMenu(view)
{
    d.view = view;
}

void UserListMenu::exec(const QPoint &pos)
{
    QModelIndex index = d.view->indexAt(d.view->mapFromGlobal(pos));
    if (index.isValid()) {
        QModelIndexList selection = d.view->selectionModel()->selectedIndexes();
        const bool multi = selection.count() > 1;
        QAction* action = addAction(tr("Whois"), this, SLOT(onWhoisTriggered()));
        action->setEnabled(!multi);
        action = addAction(tr("Query"), this, SLOT(onQueryTriggered()));
        action->setEnabled(!multi);
        addSeparator();

        const QString prefix = index.data(Irc::PrefixRole).toString();
        if (prefix.contains("@")) {
            action = addAction(multi ? tr("Deop all") : tr("Deop"), this, SLOT(onModeTriggered()));
            action->setData("-o");
        } else {
            action = addAction(multi ? tr("Op all") : tr("Op"), this, SLOT(onModeTriggered()));
            action->setData("+o");
        }

        if (prefix.contains("+")) {
            action = addAction(multi ? tr("Devoice all") : tr("Devoice"), this, SLOT(onModeTriggered()));
            action->setData("-v");
        } else {
            action = addAction(multi ? tr("Voice all") : tr("Voice"), this, SLOT(onModeTriggered()));
            action->setData("+v");
        }

        addSeparator();
        addAction(multi ? tr("Kick all") : tr("Kick"), this, SLOT(onKickTriggered()));
        addAction(multi ? tr("Ban all") : tr("Ban"), this, SLOT(onBanTriggered()));
    }
    QMenu::exec(pos);
}

void UserListMenu::onWhoisTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QModelIndexList selection = d.view->selectionModel()->selectedIndexes();
        foreach (const QModelIndex& index, selection) {
            const QString name = index.data(Irc::NameRole).toString();
            IrcCommand* command = IrcCommand::createWhois(name);
            d.view->connection()->sendCommand(command);
        }
    }
}

void UserListMenu::onQueryTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QModelIndexList selection = d.view->selectionModel()->selectedIndexes();
        foreach (const QModelIndex& index, selection) {
            const QString name = index.data(Irc::NameRole).toString();
            QMetaObject::invokeMethod(d.view, "queried", Q_ARG(QString, name));
        }
    }
}

void UserListMenu::onModeTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QModelIndexList selection = d.view->selectionModel()->selectedIndexes();
        foreach (const QModelIndex& index, selection) {
            const QString name = index.data(Irc::NameRole).toString();
            IrcCommand* command = IrcCommand::createMode(d.view->channel()->title(), action->data().toString(), name);
            d.view->connection()->sendCommand(command);
        }
    }
}

void UserListMenu::onKickTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QModelIndexList selection = d.view->selectionModel()->selectedIndexes();
        foreach (const QModelIndex& index, selection) {
            const QString name = index.data(Irc::NameRole).toString();
            IrcCommand* command = IrcCommand::createKick(d.view->channel()->title(), name);
            d.view->connection()->sendCommand(command);
        }
    }
}

void UserListMenu::onBanTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QModelIndexList selection = d.view->selectionModel()->selectedIndexes();
        foreach (const QModelIndex& index, selection) {
            const QString name = index.data(Irc::NameRole).toString();
            IrcCommand* command = IrcCommand::createMode(d.view->channel()->title(), "+b", name + "!*@*");
            d.view->connection()->sendCommand(command);
        }
    }
}
