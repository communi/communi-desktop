/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "pluginloader.h"

#include <QtPlugin>
#include "bufferplugin.h"
#include "connectionplugin.h"
#include "documentplugin.h"
#include "viewplugin.h"
#include "windowplugin.h"

Q_IMPORT_PLUGIN(AlertPlugin)
Q_IMPORT_PLUGIN(CommanderPlugin)
Q_IMPORT_PLUGIN(DockPlugin)
Q_IMPORT_PLUGIN(IgnorePlugin)
Q_IMPORT_PLUGIN(ImporterPlugin)
Q_IMPORT_PLUGIN(MonitorPlugin)
Q_IMPORT_PLUGIN(OverlayPlugin)
Q_IMPORT_PLUGIN(TrayPlugin)
Q_IMPORT_PLUGIN(VerifierPlugin)
Q_IMPORT_PLUGIN(ZncPlugin)

static QObjectList pluginInstances()
{
    // TODO: dynamic plugins
    return QPluginLoader::staticInstances();
}

PluginLoader::PluginLoader(QObject* parent) : QObject(parent)
{
}

PluginLoader* PluginLoader::instance()
{
    static PluginLoader loader;
    return &loader;
}

void PluginLoader::bufferAdded(IrcBuffer* buffer)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferPlugin* plugin = qobject_cast<BufferPlugin*>(instance);
        if (plugin)
            plugin->bufferAdded(buffer);
    }
}

void PluginLoader::bufferRemoved(IrcBuffer* buffer)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferPlugin* plugin = qobject_cast<BufferPlugin*>(instance);
        if (plugin)
            plugin->bufferRemoved(buffer);
    }
}

void PluginLoader::initConnection(IrcConnection* connection)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ConnectionPlugin* plugin = qobject_cast<ConnectionPlugin*>(instance);
        if (plugin)
            plugin->initConnection(connection);
    }
}

void PluginLoader::cleanupConnection(IrcConnection* connection)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ConnectionPlugin* plugin = qobject_cast<ConnectionPlugin*>(instance);
        if (plugin)
            plugin->cleanupConnection(connection);
    }
}

void PluginLoader::initView(BufferView* view)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ViewPlugin* plugin = qobject_cast<ViewPlugin*>(instance);
        if (plugin)
            plugin->initView(view);
    }
}

void PluginLoader::cleanupView(BufferView* view)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ViewPlugin* plugin = qobject_cast<ViewPlugin*>(instance);
        if (plugin)
            plugin->cleanupView(view);
    }
}

void PluginLoader::initDocument(TextDocument* doc)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        DocumentPlugin* plugin = qobject_cast<DocumentPlugin*>(instance);
        if (plugin)
            plugin->initDocument(doc);
    }
}

void PluginLoader::cleanupDocument(TextDocument* doc)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        DocumentPlugin* plugin = qobject_cast<DocumentPlugin*>(instance);
        if (plugin)
            plugin->cleanupDocument(doc);
    }
}

void PluginLoader::initWindow(QWidget* window)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        WindowPlugin* plugin = qobject_cast<WindowPlugin*>(instance);
        if (plugin)
            plugin->initWindow(window);
    }
}

void PluginLoader::cleanupWindow(QWidget* window)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        WindowPlugin* plugin = qobject_cast<WindowPlugin*>(instance);
        if (plugin)
            plugin->cleanupWindow(window);
    }
}
