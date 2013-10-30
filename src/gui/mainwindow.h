/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QSystemTrayIcon>

class ChatPage;
class IrcBuffer;
class IrcMessage;
class IrcSession;
class QtDockTile;
class ConnectPage;
class SoundNotification;

class MainWindow : public QStackedWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    QSize sizeHint() const;

protected:
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent* event);

private slots:
    void alert();
    void unalert();
    void doAlert();
    void doConnect();
    void onAccepted();
    void onRejected();
    void closeBuffer();
    void setCurrentBuffer(IrcBuffer* buffer);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void restoreConnections();
    void saveConnections();

private:
    struct Private {
        QIcon normalIcon;
        QIcon alertIcon;
        ChatPage* chatPage;
        QtDockTile* dockTile;
        ConnectPage* connectPage;
        SoundNotification* sound;
        QSystemTrayIcon* trayIcon;
    } d;
};

#endif // MAINWINDOW_H
