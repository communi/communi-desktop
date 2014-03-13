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

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

class IrcChannel;
class IrcUserModel;

class ListView : public QListView
{
    Q_OBJECT
    Q_PROPERTY(IrcChannel* channel READ channel WRITE setChannel NOTIFY channelChanged)

public:
    explicit ListView(QWidget* parent = 0);

    IrcChannel* channel() const;

public slots:
    void setChannel(IrcChannel* channel);

signals:
    void channelChanged(IrcChannel* channel);
    void queried(const QString& user);

public:
    QSize sizeHint() const; // TODO: protected
protected:
    void contextMenuEvent(QContextMenuEvent* event);

private slots:
    void onDoubleClicked(const QModelIndex& index);

    void onWhoisTriggered();
    void onQueryTriggered();
    void onModeTriggered();
    void onKickTriggered();
    void onBanTriggered();

private:
    QMenu* createContextMenu(const QModelIndex& index);

    struct Private {
        IrcUserModel* model;
    } d;
};

#endif // LISTVIEW_H
