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
#include <IrcChannel>

ListView::ListView(QWidget* parent) : QListView(parent)
{
    d.model = new IrcUserModel(this);
    d.model->setSortMethod(Irc::SortByTitle);

    setModel(d.model);
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
