/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QIcon>
#include <QQueue>
#include <QPointer>
#include <QMainWindow>
#include <QStackedWidget>

class ChatPage;
class IrcBuffer;
class IrcMessage;
class BufferView;
class ConnectPage;
class SettingsPage;
class IrcConnection;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    BufferView* currentView() const;
    QList<IrcConnection*> connections() const;

public slots:
    void setCurrentView(BufferView* view);
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

signals:
    void currentViewChanged(BufferView* view);
    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

protected:
    QSize sizeHint() const;
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
        QPointer<BufferView> view;
        SettingsPage* settingsPage;
        IrcConnection* editedConnection;
        QList<IrcConnection*> connections;
        QQueue<IrcConnection*> restoredConnections;
    } d;
};

#endif // MAINWINDOW_H
