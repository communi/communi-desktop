/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
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

protected:
    QSize sizeHint() const;
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
