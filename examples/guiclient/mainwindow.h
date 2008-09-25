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
#include "ircsession.h"

class MessageView;
class QTabWidget;
class HistoryLineEdit;
class QCompleter;
class QShortcut;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    bool eventFilter(QObject* object, QEvent* event);

protected slots:
    void on_connected();
    void on_disconnected();
    void on_nickChanged(const QString& origin, const QString& nick);
    void on_quit(const QString& origin, const QString& message);
    void on_joined(const QString& origin, const QString& channel);
    void on_parted(const QString& origin, const QString& channel, const QString& message);
    void on_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args);
    void on_userModeChanged(const QString& origin, const QString& mode);
    void on_topicChanged(const QString& origin, const QString& channel, const QString& topic);
    void on_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message);
    void on_channelMessageReceived(const QString& origin, const QString& channel, const QString& message);
    void on_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_noticeReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_invited(const QString& origin, const QString& nick, const QString& channel);
    void on_ctcpRequestReceived(const QString& origin, const QString& message);
    void on_ctcpReplyReceived(const QString& origin, const QString& message);
    void on_ctcpActionReceived(const QString& origin, const QString& receiver, const QString& message);
    void on_unknownMessageReceived(const QString& origin, const QStringList& params);
    void on_numericMessageReceived(const QString& origin, uint event, const QStringList& params);

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
    void autoComplete();
    void textEdited();
    void insertCompletion(const QString& completion);

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
    QList<int> alertIndices;
    QCompleter* completer;

    IrcSession session;
    ConnectDialog connectDialog;
    QShortcut* zoomInShortcut;
    QShortcut* zoomOutShortcut;
};

#endif // MAINWINDOW_H
