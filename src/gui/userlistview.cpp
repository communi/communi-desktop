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
#include "userlistmenu.h"
#include "styledscrollbar.h"
#include "itemdelegate.h"
#include <IrcChannel>
#include <IrcConnection>
#include <IrcUserModel>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QContextMenuEvent>
#include <QScrollBar>
#include <QAction>

UserListView::UserListView(QWidget* parent) : QListView(parent)
{
    setItemDelegate(new ItemDelegate(this));
    connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));

    setVerticalScrollBar(new StyledScrollBar(this));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

UserListView::~UserListView()
{
}

QSize UserListView::sizeHint() const
{
    return QSize(16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width(), QListView::sizeHint().height());
}

IrcConnection* UserListView::connection() const
{
    return d.connection;
}

void UserListView::setConnection(IrcConnection* connection)
{
    d.connection = connection;
}

IrcChannel* UserListView::channel() const
{
    return d.channel;
}

void UserListView::setChannel(IrcChannel* channel)
{
    if (d.channel != channel) {
        d.channel = channel;
        if (!d.userModel) {
            d.userModel = new IrcUserModel(channel);
            d.userModel->setSortMethod(Irc::SortByTitle);
            setModel(d.userModel);
        }
        d.userModel->setChannel(channel);
    }
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

void UserListView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        UserListMenu menu(this);
        menu.exec(event->globalPos());
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
