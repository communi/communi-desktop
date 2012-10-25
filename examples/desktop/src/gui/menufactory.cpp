/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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
#include "sessiontreeitem.h"
#include "sessiontreewidget.h"
#include "session.h"
#include <IrcCommand>

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
            listView->session()->sendCommand(command);
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
            IrcCommand* command = IrcCommand::createMode(listView->channel(), params.at(1), params.at(0));
            listView->session()->sendCommand(command);
        }
    }

    void onKickTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            IrcCommand* command = IrcCommand::createKick(listView->channel(), action->data().toString());
            listView->session()->sendCommand(command);
        }
    }

    void onBanTriggered()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            IrcCommand* command = IrcCommand::createMode(listView->channel(), "+b", action->data().toString() + "!*@*");
            listView->session()->sendCommand(command);
        }
    }

private:
    UserListView* listView;
};

QMenu* MenuFactory::createUserListMenu(const QString& user, UserListView* listView)
{
    UserListMenu* menu = new UserListMenu(listView);

    QString mode = listView->session()->userPrefix(user);
    QString name = listView->session()->unprefixedUser(user);

    QAction* action = 0;
    action = menu->addAction(tr("Whois"), menu, SLOT(onWhoisTriggered()));
    action->setData(name);

    action = menu->addAction(tr("Query"), menu, SLOT(onQueryTriggered()));
    action->setData(name);

    menu->addSeparator();

    if (mode.contains("@")) {
        action = menu->addAction(tr("Deop"), menu, SLOT(onModeTriggered()));
        action->setData(QStringList() << name << "-o");
    } else {
        action = menu->addAction(tr("Op"), menu, SLOT(onModeTriggered()));
        action->setData(QStringList() << name << "+o");
    }

    if (mode.contains("+")) {
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

class SessionTreeMenu : public QMenu
{
    Q_OBJECT

public:
    SessionTreeMenu(SessionTreeItem* item, SessionTreeWidget* tree) :
        QMenu(tree), item(item), tree(tree)
    {
    }

private slots:
    void onEditSession()
    {
        QMetaObject::invokeMethod(tree, "editSession", Q_ARG(Session*, item->session()));
    }

    void onWhoisTriggered()
    {
        IrcCommand* command = IrcCommand::createWhois(item->text(0));
        item->session()->sendCommand(command);
    }

    void onCloseItem()
    {
        QMetaObject::invokeMethod(tree, "closeItem", Q_ARG(SessionTreeItem*, item));
    }

private:
    SessionTreeItem* item;
    SessionTreeWidget* tree;
};

QMenu* MenuFactory::createSessionTreeMenu(SessionTreeItem* item, SessionTreeWidget* tree)
{
    SessionTreeMenu* menu = new SessionTreeMenu(item, tree);
    if (!item->parent()) {
        if (item->session()->isActive())
            menu->addAction(tr("Disconnect"), item->session(), SLOT(quit()));
        else
            menu->addAction(tr("Reconnect"), item->session(), SLOT(reconnect()));
        menu->addAction(tr("Edit"), menu, SLOT(onEditSession()))->setEnabled(!item->session()->isActive());
    }
    if (item->session()->isChannel(item->text(0))) {
        menu->addAction(tr("Part"), menu, SLOT(onCloseItem()));
    } else {
        menu->addAction(tr("Whois"), menu, SLOT(onWhoisTriggered()));
        menu->addAction(tr("Close"), menu, SLOT(onCloseItem()));
    }
    return menu;
}

#include "menufactory.moc"
