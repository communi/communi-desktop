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

#include "menufactory.h"
#include "userlistview.h"
#include "connection.h"
#include <IrcCommand>
#include <IrcChannel>

MenuFactory::MenuFactory(QObject* parent) : QObject(parent)
{
}

MenuFactory::~MenuFactory()
{
}

class UserListMenu : public QMenu
{
    Q_OBJECT

public:
    UserListMenu(UserListView* listView) : QMenu(listView), listView(listView)
    {
    }

private slots:
    void onWhoisTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            IrcCommand* command = IrcCommand::createWhois(action->data().toString());
            listView->connection()->sendCommand(command);
        }
    }

    void onQueryTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action)
            QMetaObject::invokeMethod(listView, "queried", Q_ARG(QString, action->data().toString()));
    }

    void onModeTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            QStringList params = action->data().toStringList();
            IrcCommand* command = IrcCommand::createMode(listView->channel()->title(), params.at(1), params.at(0));
            listView->connection()->sendCommand(command);
        }
    }

    void onKickTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            IrcCommand* command = IrcCommand::createKick(listView->channel()->title(), action->data().toString());
            listView->connection()->sendCommand(command);
        }
    }

    void onBanTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            IrcCommand* command = IrcCommand::createMode(listView->channel()->title(), "+b", action->data().toString() + "!*@*");
            listView->connection()->sendCommand(command);
        }
    }

private:
    UserListView* listView;
};

QMenu* MenuFactory::createUserListMenu(const QString& prefix, const QString& name, UserListView* listView)
{
    UserListMenu* menu = new UserListMenu(listView);

    QAction* action = 0;
    action = menu->addAction(tr("Whois"), menu, SLOT(onWhoisTriggered()));
    action->setData(name);

    action = menu->addAction(tr("Query"), menu, SLOT(onQueryTriggered()));
    action->setData(name);

    menu->addSeparator();

    if (prefix.contains("@")) {
        action = menu->addAction(tr("Deop"), menu, SLOT(onModeTriggered()));
        action->setData(QStringList() << name << "-o");
    } else {
        action = menu->addAction(tr("Op"), menu, SLOT(onModeTriggered()));
        action->setData(QStringList() << name << "+o");
    }

    if (prefix.contains("+")) {
        action = menu->addAction(tr("Devoice"), menu, SLOT(onModeTriggered()));
        action->setData(QStringList() << name << "-v");
    } else {
        action = menu->addAction(tr("Voice"), menu, SLOT(onModeTriggered()));
        action->setData(QStringList() << name << "+v");
    }

    menu->addSeparator();

    action = menu->addAction(tr("Kick"), menu, SLOT(onKickTriggered()));
    action->setData(name);

    action = menu->addAction(tr("Ban"), menu, SLOT(onBanTriggered()));
    action->setData(name);

    return menu;
}

#include "menufactory.moc"
