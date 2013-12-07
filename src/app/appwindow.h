/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "mainwindow.h"
#include <QIcon>
#include <QQueue>
#include <QStackedWidget>

class ChatPage;
class IrcBuffer;
class IrcMessage;
class ConnectPage;
class SettingsPage;
class IrcConnection;

class AppWindow : public MainWindow
{
    Q_OBJECT

public:
    AppWindow(QWidget* parent = 0);
    ~AppWindow();

    QSize sizeHint() const;

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void doConnect();
    void onConnectAccepted();
    void onSettingsAccepted();
    void onRejected();
    void updateTitle();
    void showSettings();
    void editConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);
    void restoreConnection(IrcConnection* connection = 0);
    void delayedRestoreConnection();

private:
    struct Private {
        QIcon normalIcon;
        QIcon alertIcon;
        ChatPage* chatPage;
        QStackedWidget* stack;
        ConnectPage* connectPage;
        SettingsPage* settingsPage;
        IrcConnection* editedConnection;
        QQueue<IrcConnection*> restoredConnections;
    } d;
};

#endif // APPWINDOW_H
