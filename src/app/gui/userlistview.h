/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include <QListView>

class IrcUser;
class IrcChannel;
class IrcUserModel;

class UserListView : public QListView
{
    Q_OBJECT

public:
    UserListView(QWidget* parent = 0);
    ~UserListView();

    IrcChannel* channel() const;

public slots:
    void setChannel(IrcChannel* channel);

signals:
    void doubleClicked(const QString& user);

protected:
    QSize sizeHint() const;
    void contextMenuEvent(QContextMenuEvent* event);

private slots:
    void onDoubleClicked(const QModelIndex& index);

private:
    struct Private {
        IrcUserModel* model;
    } d;
};

#endif // USERLISTVIEW_H
