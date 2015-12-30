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

#include "pluginloader.h"

#include <QDir>
#include <QFileInfo>
#include <QApplication>

#include <QtPlugin>
#include "bufferview.h"
#include "bufferplugin.h"
#include "connectionplugin.h"
#include "dockplugin.h"
#include "documentplugin.h"
#include "themeplugin.h"
#include "viewplugin.h"
#include "windowplugin.h"

static QObjectList loadPlugins(const QStringList& paths)
{
    QObjectList instances;
    foreach (const QString& path, paths) {
        foreach (const QFileInfo& file, QDir(path).entryInfoList(QDir::Files)) {
            const QString base = file.baseName();
            // blacklisted obsolete plugins
            if (base.startsWith("monitorplugin") || base.startsWith("libmonitorplugin"))
                continue;
#if defined(Q_OS_WIN)
            // avoid loading undesired files from %QTDIR%\bin
            if (base.startsWith("Qt5") || base.startsWith("Irc") || base.startsWith("Enginio")
                    || base.startsWith("icu") || file.suffix() != "dll")
                continue;
#elif defined(Q_OS_UNIX)
            // avoid trying to load the whole /usr/bin
            if (!base.startsWith("lib"))
                continue;
#endif
            QPluginLoader loader(file.absoluteFilePath());
            if (loader.load())
                instances += loader.instance();
        }
    }
    return instances;
}

static QObjectList pluginInstances()
{
    static QObjectList instances = loadPlugins(QApplication::libraryPaths());
    return instances;
}

QStringList PluginLoader::paths()
{
    QStringList lst;
    lst += COMMUNI_INSTALL_PLUGINS;
    QDir dir(QApplication::applicationDirPath());
#ifdef Q_OS_MAC
    if (dir.dirName() == "MacOS" && dir.cd("../PlugIns"))
        lst += dir.absolutePath();
    else
#endif
    if (dir.cd("communi") || (dir.dirName() == "bin" && dir.cd("../plugins")))
        lst += dir.absolutePath();
    return lst;
}

PluginLoader::PluginLoader(QObject* parent) : QObject(parent)
{
    qRegisterMetaType<BufferView*>();
}

PluginLoader* PluginLoader::instance()
{
    static PluginLoader loader;
    return &loader;
}

#define COMMUNI_PLUGIN_CALL(T, F) \
    foreach (QObject* instance, pluginInstances()) { \
        T* plugin = qobject_cast<T*>(instance); \
        if (plugin) \
            plugin->F; \
    }

void PluginLoader::bufferAdded(IrcBuffer* buffer)
{
    COMMUNI_PLUGIN_CALL(BufferPlugin, bufferAdded(buffer))
}

void PluginLoader::bufferRemoved(IrcBuffer* buffer)
{
    COMMUNI_PLUGIN_CALL(BufferPlugin, bufferRemoved(buffer))
}

void PluginLoader::connectionAdded(IrcConnection* connection)
{
    COMMUNI_PLUGIN_CALL(ConnectionPlugin, connectionAdded(connection))
}

void PluginLoader::connectionRemoved(IrcConnection* connection)
{
    COMMUNI_PLUGIN_CALL(ConnectionPlugin, connectionRemoved(connection))
}

void PluginLoader::viewAdded(BufferView* view)
{
    COMMUNI_PLUGIN_CALL(ViewPlugin, viewAdded(view))
}

void PluginLoader::viewRemoved(BufferView* view)
{
    COMMUNI_PLUGIN_CALL(ViewPlugin, viewRemoved(view))
}

void PluginLoader::documentAdded(TextDocument* doc)
{
    COMMUNI_PLUGIN_CALL(DocumentPlugin, documentAdded(doc))
}

void PluginLoader::documentRemoved(TextDocument* doc)
{
    COMMUNI_PLUGIN_CALL(DocumentPlugin, documentRemoved(doc))
}

void PluginLoader::themeChanged(const ThemeInfo& theme)
{
    COMMUNI_PLUGIN_CALL(ThemePlugin, themeChanged(theme))
}

void PluginLoader::windowCreated(QMainWindow* window)
{
    COMMUNI_PLUGIN_CALL(WindowPlugin, windowCreated(window))
}

void PluginLoader::windowDestroyed(QMainWindow* window)
{
    COMMUNI_PLUGIN_CALL(WindowPlugin, windowDestroyed(window))
}

void PluginLoader::dockAlert(IrcMessage* message)
{
    COMMUNI_PLUGIN_CALL(DockPlugin, dockAlert(message))
}

void PluginLoader::setupTrayIcon(QSystemTrayIcon* tray)
{
    COMMUNI_PLUGIN_CALL(DockPlugin, setupTrayIcon(tray))
}

void PluginLoader::setupMuteAction(QAction* action)
{
    COMMUNI_PLUGIN_CALL(DockPlugin, setupMuteAction(action))
}
