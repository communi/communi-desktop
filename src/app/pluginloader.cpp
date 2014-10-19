/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "pluginloader.h"

#include <QDir>
#include <QApplication>

#include <QtPlugin>
#include "bufferview.h"
#include "bufferplugin.h"
#include "connectionplugin.h"
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
            // blacklisted obsolete plugin
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
#ifdef Q_OS_MAC
    QDir dir(QApplication::applicationFilePath());
    if (dir.cd("../../PlugIns"))
        lst += dir.absolutePath();
#endif
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

void PluginLoader::bufferAdded(IrcBuffer* buffer)
{
    foreach (QObject* instance, pluginInstances()) {
        BufferPlugin* plugin = qobject_cast<BufferPlugin*>(instance);
        if (plugin)
            plugin->bufferAdded(buffer);
    }
}

void PluginLoader::bufferRemoved(IrcBuffer* buffer)
{
    foreach (QObject* instance, pluginInstances()) {
        BufferPlugin* plugin = qobject_cast<BufferPlugin*>(instance);
        if (plugin)
            plugin->bufferRemoved(buffer);
    }
}

void PluginLoader::connectionAdded(IrcConnection* connection)
{
    foreach (QObject* instance, pluginInstances()) {
        ConnectionPlugin* plugin = qobject_cast<ConnectionPlugin*>(instance);
        if (plugin)
            plugin->connectionAdded(connection);
    }
}

void PluginLoader::connectionRemoved(IrcConnection* connection)
{
    foreach (QObject* instance, pluginInstances()) {
        ConnectionPlugin* plugin = qobject_cast<ConnectionPlugin*>(instance);
        if (plugin)
            plugin->connectionRemoved(connection);
    }
}

void PluginLoader::viewAdded(BufferView* view)
{
    foreach (QObject* instance, pluginInstances()) {
        ViewPlugin* plugin = qobject_cast<ViewPlugin*>(instance);
        if (plugin)
            plugin->viewAdded(view);
    }
}

void PluginLoader::viewRemoved(BufferView* view)
{
    foreach (QObject* instance, pluginInstances()) {
        ViewPlugin* plugin = qobject_cast<ViewPlugin*>(instance);
        if (plugin)
            plugin->viewRemoved(view);
    }
}

void PluginLoader::documentAdded(TextDocument* doc)
{
    foreach (QObject* instance, pluginInstances()) {
        DocumentPlugin* plugin = qobject_cast<DocumentPlugin*>(instance);
        if (plugin)
            plugin->documentAdded(doc);
    }
}

void PluginLoader::documentRemoved(TextDocument* doc)
{
    foreach (QObject* instance, pluginInstances()) {
        DocumentPlugin* plugin = qobject_cast<DocumentPlugin*>(instance);
        if (plugin)
            plugin->documentRemoved(doc);
    }
}

void PluginLoader::themeChanged(const ThemeInfo& theme)
{
    foreach (QObject* instance, pluginInstances()) {
        ThemePlugin* plugin = qobject_cast<ThemePlugin*>(instance);
        if (plugin)
            plugin->themeChanged(theme);
    }
}

void PluginLoader::windowCreated(QMainWindow* window)
{
    foreach (QObject* instance, pluginInstances()) {
        WindowPlugin* plugin = qobject_cast<WindowPlugin*>(instance);
        if (plugin)
            plugin->windowCreated(window);
    }
}

void PluginLoader::windowDestroyed(QMainWindow* window)
{
    foreach (QObject* instance, pluginInstances()) {
        WindowPlugin* plugin = qobject_cast<WindowPlugin*>(instance);
        if (plugin)
            plugin->windowDestroyed(window);
    }
}
