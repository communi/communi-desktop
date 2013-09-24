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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "trayicon.h"

class SessionStackView;
class SessionTreeWidget;
class SoundNotification;
class SessionTreeItem;
struct ConnectionInfo;
class MessageView;
class IrcMessage;
class QtDockTile;
class IrcConnection;
class QShortcut;
class HomePage;
class Overlay;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    QSize sizeHint() const;

public slots:
    void connectTo(const QString& host = QString(), quint16 port = 6667,
                   const QString& nick = QString(), const QString& password = QString());
    void connectTo(const ConnectionInfo& connection);
    void connectToImpl(const ConnectionInfo& connection);

protected:
    void closeEvent(QCloseEvent* event);
    void changeEvent(QEvent* event);

private slots:
    void initialize();
    void editConnection(IrcConnection* connection);
    void applySettings();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void highlighted(IrcMessage* message);
    void missed(IrcMessage* message);
    void viewAdded(MessageView* view);
    void viewActivated(MessageView* view);
    void closeTreeItem(SessionTreeItem* item);
    void currentTreeItemChanged(IrcConnection* connection, const QString& view);
    void splitterChanged(const QByteArray& state);
    void updateOverlay();
    void reconnect();
    void addView();
    void closeView();
    void searchView();

private:
    void createTree();
    void createHome();

    SessionTreeWidget* treeWidget;
    SessionStackView* stackView;
    TrayIcon* trayIcon;
    QAction* muteAction;
    QtDockTile* dockTile;
    QShortcut* searchShortcut;
    QPointer<Overlay> overlay;
    SoundNotification* sound;
    HomePage *homePage;
};

#endif // MAINWINDOW_H
