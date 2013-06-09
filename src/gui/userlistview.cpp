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
#include <IrcBuffer>
#include <IrcUserModel>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QContextMenuEvent>
#include <QScrollBar>
#include <QAction>

UserListView::UserListView(QWidget* parent) : QListView(parent)
{
    d.buffer = 0;
    d.session = 0;
    d.userModel = 0;
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

IrcSession* UserListView::session() const
{
    return d.session;
}

void UserListView::setSession(IrcSession* session)
{
    d.session = session;
}

IrcBuffer* UserListView::buffer() const
{
    return d.buffer;
}

void UserListView::setBuffer(IrcBuffer* buffer)
{
    d.buffer = buffer;
    d.userModel = new IrcUserModel(buffer);
    SortedUserModel* sortedModel = new SortedUserModel(d.userModel);
    sortedModel->setBuffer(buffer);
    setModel(sortedModel);
}

IrcUserModel* UserListView::userModel() const
{
    return d.userModel;
}

bool UserListView::hasUser(const QString& user) const
{
    if (d.userModel)
        return d.userModel->contains(user);
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
        QMenu* menu = menuFactory()->createUserListMenu(index.data(Irc::PrefixRole).toString(),
                                                        index.data(Irc::NameRole).toString(), this);
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
