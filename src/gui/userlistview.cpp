/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "userlistview.h"
#include <IrcUserModel>
#include <QScrollBar>
#include <IrcChannel>
#include <IrcUser>

UserListView::UserListView(QWidget* parent) : QListView(parent)
{
    d.model = new IrcUserModel(this);
    d.model->setSortMethod(Irc::SortByTitle);

    setModel(d.model);
    setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
}

UserListView::~UserListView()
{
}

IrcChannel* UserListView::channel() const
{
    return d.model->channel();
}

void UserListView::setChannel(IrcChannel* channel)
{
    d.model->setChannel(channel);
}

QSize UserListView::sizeHint() const
{
    const int w = 16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width();
    return QSize(w, QListView::sizeHint().height());
}

void UserListView::contextMenuEvent(QContextMenuEvent* event)
{
    Q_UNUSED(event);
    // TODO
}

void UserListView::onDoubleClicked(const QModelIndex& index)
{
    if (index.isValid())
        emit doubleClicked(index.data(Irc::NameRole).toString());
}
