/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#ifndef Q_OS_SYMBIAN
#include "trayicon.h"
#endif // Q_OS_SYMBIAN

class QDBusInterface;
class MainTabWidget;
struct Connection;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    QSize sizeHint() const;

public slots:
    void connectTo(const QString& host = QString(), quint16 port = 6667,
                   const QString& nick = QString(), const QString& password = QString());
    void connectTo(const Connection& connection);
    void connectToImpl(const Connection& connection);
    void disconnectFrom(const QString& message);
    void quit(const QString& message);

protected:
    void closeEvent(QCloseEvent* event);
    void changeEvent(QEvent* event);
    bool event(QEvent* event);

private slots:
    void initialize();
#ifndef Q_OS_SYMBIAN
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
#endif // Q_OS_SYMBIAN
    void activateAlert(bool activate);
    void activateVibra(bool activate);
    void tabActivated(int index);
    void createWelcomeView();
    void createTabbedView();

private:
    MainTabWidget* tabWidget;
#ifndef Q_OS_SYMBIAN
    TrayIcon* trayIcon;
#endif // Q_OS_SYMBIAN
#ifdef Q_WS_MAEMO_5
    QDBusInterface* interface;
    QAction* networksAction;
    QAction* channelsAction;
#endif // Q_WS_MAEMO_5
};

#endif // MAINWINDOW_H
