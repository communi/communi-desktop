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
class SystemMonitor;

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

public slots:
    void saveState();
    void restoreState();

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

private:
    struct Private {
        bool save;
        Dock* dock;
        ChatPage* chatPage;
        QStackedWidget* stack;
        SystemMonitor* monitor;
        QPointer<BufferView> view;
        QList<IrcConnection*> connections;
    } d;
};

#endif // MAINWINDOW_H
