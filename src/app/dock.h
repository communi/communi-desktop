/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef DOCK_H
#define DOCK_H

#include <QObject>
#include <QSystemTrayIcon>

class Alert;
class MainWindow;
class QtDockTile;
class IrcMessage;
class IrcConnection;

class Dock : public QObject
{
    Q_OBJECT

public:
    explicit Dock(MainWindow* window);

public slots:
    void alert(IrcMessage* message);

private slots:
    void onConnectionAdded(IrcConnection* connection);
    void onConnectionRemoved(IrcConnection* connection);

    void updateBadge();
    void updateTray();

    void onWindowActivated();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    struct Private {
        bool blink;
        bool blinking;
        QIcon alertIcon;
        QIcon onlineIcon;
        QIcon offlineIcon;
        MainWindow* window;
        QSystemTrayIcon* tray;
        QtDockTile* dock;
        Alert* alert;
    } d;
};

#endif // DOCK_H
