/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "listview.h"
#include <IrcUserModel>
#include <QFontMetrics>
#include <QScrollBar>
#include <IrcChannel>
#include <Irc>

ListView::ListView(QWidget* parent) : QListView(parent)
{
    setFocusPolicy(Qt::NoFocus);

    d.model = new IrcUserModel(this);
    d.model->setSortMethod(Irc::SortByTitle);
    setModel(d.model);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
}

IrcChannel* ListView::channel() const
{
    return d.model->channel();
}

void ListView::setChannel(IrcChannel* channel)
{
    if (d.model->channel() != channel) {
        d.model->setChannel(channel);
        emit channelChanged(channel);
    }
}

QSize ListView::sizeHint() const
{
    const int w = 16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width();
    return QSize(w, QListView::sizeHint().height());
}

void ListView::onDoubleClicked(const QModelIndex& index)
{
    if (index.isValid())
        emit queried(index.data(Irc::NameRole).toString());
}
