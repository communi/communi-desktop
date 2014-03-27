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

#include <QQueue>
#include <QPointer>
#include <QMainWindow>
#include <QStackedWidget>

class Dock;
class ChatPage;
class IrcBuffer;
class IrcMessage;
class BufferView;
class IrcConnection;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(BufferView* currentView READ currentView WRITE setCurrentView)
    Q_PROPERTY(QList<IrcConnection*> connections READ connections)

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    BufferView* currentView() const;
    QList<IrcConnection*> connections() const;

    void saveState();
    void restoreState();

public slots:
    void setCurrentView(BufferView* view);
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

signals:
    void activated();
    void currentViewChanged(BufferView* view);
    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

protected slots:
    void push(QWidget* page);
    void pop();

protected:
    QSize sizeHint() const;
    bool event(QEvent* event);
    void closeEvent(QCloseEvent* event);

private slots:
    void doConnect();
    void onEditAccepted();
    void onConnectAccepted();
    void onSettingsAccepted();
    void updateTitle();
    void showSettings();
    void showHelp();
    void editConnection(IrcConnection* connection);
    void restoreConnection(IrcConnection* connection = 0);
    void delayedRestoreConnection();

private:
    struct Private {
        Dock* dock;
        ChatPage* chatPage;
        QStackedWidget* stack;
        QPointer<BufferView> view;
        QList<IrcConnection*> connections;
        QQueue<IrcConnection*> restoredConnections;
    } d;
};

#endif // MAINWINDOW_H
