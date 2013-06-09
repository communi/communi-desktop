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

#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include <QListView>

class IrcBuffer;
class IrcSession;
class IrcMessage;
class IrcCommand;
class MenuFactory;
class IrcUserModel;

class UserListView : public QListView
{
    Q_OBJECT

public:
    UserListView(QWidget* parent = 0);
    ~UserListView();

    QSize sizeHint() const;

    IrcSession* session() const;
    void setSession(IrcSession* session);

    IrcBuffer* buffer() const;
    void setBuffer(IrcBuffer* buffer);

    IrcUserModel* userModel() const;
    bool hasUser(const QString& user) const;

    MenuFactory* menuFactory() const;
    void setMenuFactory(MenuFactory* factory);

signals:
    void queried(const QString& user);
    void doubleClicked(const QString& user);
    void commandRequested(IrcCommand* command);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);

private slots:
    void onDoubleClicked(const QModelIndex& index);

private:
    struct Private {
        IrcBuffer* buffer;
        IrcSession* session;
        IrcUserModel* userModel;
        MenuFactory* menuFactory;
    } d;
};

#endif // USERLISTVIEW_H
