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

#include "userlistview.h"
#include "sortedusermodel.h"
#include "itemdelegate.h"
#include "menufactory.h"
#include "session.h"
#include <IrcChannel>
#include <IrcUserModel>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QContextMenuEvent>
#include <QScrollBar>
#include <QAction>

UserListView::UserListView(QWidget* parent) : QListView(parent)
{
    d.session = 0;
    d.channel = 0;
    d.menuFactory = 0;
    setItemDelegate(new ItemDelegate(this));
    connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));
}

UserListView::~UserListView()
{
}

QSize UserListView::sizeHint() const
{
    return QSize(16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width(), QListView::sizeHint().height());
}

Session* UserListView::session() const
{
    return d.session;
}

void UserListView::setSession(Session* session)
{
    d.session = session;
}

IrcChannel* UserListView::channel() const
{
    return d.channel;
}

void UserListView::setChannel(IrcChannel* channel)
{
    d.channel = channel;
    SortedUserModel* sortedModel = new SortedUserModel(channel->model());
    sortedModel->sortByPrefixes(IrcSessionInfo(d.session).prefixes());
    setModel(sortedModel);
}

IrcUserModel* UserListView::userModel() const
{
    if (d.channel)
        return d.channel->model();
    return 0;
}

bool UserListView::hasUser(const QString& user) const
{
    if (IrcUserModel* model = userModel())
        return model->names().contains(user, Qt::CaseInsensitive);
    return false;
}

MenuFactory* UserListView::menuFactory() const
{
    if (!d.menuFactory) {
        UserListView* that = const_cast<UserListView*>(this);
        that->d.menuFactory = new MenuFactory(that);
    }
    return d.menuFactory;
}

void UserListView::setMenuFactory(MenuFactory* factory)
{
    if (d.menuFactory && d.menuFactory->parent() == this)
        delete d.menuFactory;
    d.menuFactory = factory;
}

void UserListView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        QMenu* menu = menuFactory()->createUserListMenu(index.data().toString(), this);
        menu->exec(event->globalPos());
        menu->deleteLater();
    }
}

void UserListView::mousePressEvent(QMouseEvent* event)
{
    QListView::mousePressEvent(event);
    if (!indexAt(event->pos()).isValid())
        selectionModel()->clear();
}

void UserListView::onDoubleClicked(const QModelIndex& index)
{
    if (index.isValid())
        emit doubleClicked(index.data(Irc::NameRole).toString());
}
