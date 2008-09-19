/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2008 Adam Higerd ahigerd@libqxt.org
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include "connectdialog.h"

class IrcSession;
class MessageView;
class QTabWidget;
class HistoryLineEdit;
class QThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    bool eventFilter(QObject* object, QEvent* event);

protected slots:
    void on_irc_connected();
    void on_irc_disconnected();
    void on_irc_nickChanged(const QString& origin, const QString& nick);
    void on_irc_quit(const QString& origin, const QString& message);
    void on_irc_joined(const QString& origin, const QString& channel);
    void on_irc_parted(const QString& origin, const QString& channel, const QString& message);
    void on_irc_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args);
    void on_irc_userModeChanged(const QString& origin, const QString& mode);
    void on_irc_topicChanged(const QString& origin, const QString& channel, const QString& topic);
    void on_irc_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message);
    void on_irc_channelMessageReceived(const QString& origin, const QString& channel, const QString& message);
    void on_irc_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_irc_noticeReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_irc_invited(const QString& origin, const QString& nick, const QString& channel);
    void on_irc_ctcpRequestReceived(const QString& origin, const QString& message);
    void on_irc_ctcpReplyReceived(const QString& origin, const QString& message);
    void on_irc_ctcpActionReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_irc_unknownMessageReceived(const QString& origin, const QStringList& params);
    void on_irc_numericMessageReceived(const QString& origin, uint event, const QStringList& params);

private slots:
    void initialize();
    void partCurrentChannel();
    void tabActivated(int index);
    void moveToNextTab();
    void moveToPrevTab();
    void moveToNextPage();
    void moveToPrevPage();
    void send();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void alert();

private:
    QString prepareTarget(const QString& sender, const QString& receiver);

    QWidget* centralwidget;
    QGridLayout* gridLayout;
    QTabWidget* tabWidget;
    HistoryLineEdit* lineEdit;
    QPushButton* pushButton;
    QMenuBar* menubar;
    QHash<QString, MessageView*> views;
    QSystemTrayIcon* trayIcon;
    QTimer* alertTimer;

    QThread* thread;
    IrcSession* session;
    ConnectDialog connectDialog;
};

#endif // MAINWINDOW_H
