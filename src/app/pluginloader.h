/*
  Copyright (C) 2008-2016 The Communi Project

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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QPluginLoader>
#include <QMap>

class IrcBuffer;
class ThemeInfo;
class BufferView;
class IrcMessage;
class TextDocument;
class IrcConnection;

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMainWindow)
QT_FORWARD_DECLARE_CLASS(QSystemTrayIcon)
QT_FORWARD_DECLARE_CLASS(QShowEvent)

class PluginLoader : public QObject
{
    Q_OBJECT

public:
    static PluginLoader* instance();

    static QStringList paths();

    void enablePlugin(const QString &plugin);
    void disablePlugin(const QString &plugin);

public slots:
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);

    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

    void viewAdded(BufferView* view);
    void viewRemoved(BufferView* view);

    void documentAdded(TextDocument* doc);
    void documentRemoved(TextDocument* doc);

    void themeChanged(const ThemeInfo& theme);

    void windowCreated(QMainWindow* window);
    void windowDestroyed(QMainWindow* window);
    void windowShowEvent(QMainWindow* window, QShowEvent *event);

    void dockAlert(IrcMessage* message);
    void setupTrayIcon(QSystemTrayIcon* tray);
    void setupMuteAction(QAction* action);

    void settingsChanged();

private:
    PluginLoader(QObject* parent = 0);
    struct Private {
        QMap<QString, QObject*> enabledPlugins;
        QMap<QString, QObject*> disabledPlugins;
    } d;
};

#endif // PLUGINLOADER_H
