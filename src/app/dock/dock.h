/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DOCK_H
#define DOCK_H

#include <QObject>
#include <QAction>
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
    ~Dock();

    void init();
    void uninit();

public slots:
    void alert(IrcMessage* message);

private slots:
    void onConnectionAdded(IrcConnection* connection);
    void onConnectionRemoved(IrcConnection* connection);

    void updateBadge();
    void updateTray();

    void activateAlert();
    void deactivateAlert();

    void onWindowActivated();
    void onMuteToggled(bool mute);
    void onOfflineToggled(bool offline);
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onTrayMessageClicked();

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
        QAction* muteAction;
        QAction* offlineAction;
        Alert* alert;
        bool active;
    } d;
};

#endif // DOCK_H
