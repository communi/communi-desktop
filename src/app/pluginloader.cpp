/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "pluginloader.h"

#include <QtPlugin>
#include <QPluginLoader>
#include "bufferplugin.h"
#include "bufferviewplugin.h"
#include "connectionplugin.h"
#include "textdocumentplugin.h"
#include "treewidgetplugin.h"
#include "mainwindowplugin.h"

Q_IMPORT_PLUGIN(CommanderPlugin)
Q_IMPORT_PLUGIN(DockPlugin)
Q_IMPORT_PLUGIN(FinderPlugin)
Q_IMPORT_PLUGIN(IgnorePlugin)
Q_IMPORT_PLUGIN(ImporterPlugin)
Q_IMPORT_PLUGIN(MonitorPlugin)
Q_IMPORT_PLUGIN(SoundPlugin)
Q_IMPORT_PLUGIN(TitlePlugin)
Q_IMPORT_PLUGIN(TrayPlugin)
Q_IMPORT_PLUGIN(TriconPlugin)
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

void PluginLoader::initWindow(MainWindow* window)
{
    foreach (QObject* instance, pluginInstances()) {
        MainWindowPlugin* plugin = qobject_cast<MainWindowPlugin*>(instance);
        if (plugin)
            plugin->initWindow(window);
    }
}

void PluginLoader::cleanupWindow(MainWindow* window)
{
    foreach (QObject* instance, pluginInstances()) {
        MainWindowPlugin* plugin = qobject_cast<MainWindowPlugin*>(instance);
        if (plugin)
            plugin->cleanupWindow(window);
    }
}

void PluginLoader::initBuffer(IrcBuffer* buffer)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferPlugin* plugin = qobject_cast<BufferPlugin*>(instance);
        if (plugin)
            plugin->initBuffer(buffer);
    }
}

void PluginLoader::cleanupBuffer(IrcBuffer* buffer)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferPlugin* plugin = qobject_cast<BufferPlugin*>(instance);
        if (plugin)
            plugin->cleanupBuffer(buffer);
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
        BufferViewPlugin* plugin = qobject_cast<BufferViewPlugin*>(instance);
        if (plugin)
            plugin->initView(view);
    }
}

void PluginLoader::cleanupView(BufferView* view)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferViewPlugin* plugin = qobject_cast<BufferViewPlugin*>(instance);
        if (plugin)
            plugin->cleanupView(view);
    }
}

void PluginLoader::initDocument(TextDocument* doc)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TextDocumentPlugin* plugin = qobject_cast<TextDocumentPlugin*>(instance);
        if (plugin)
            plugin->initDocument(doc);
    }
}

void PluginLoader::cleanupDocument(TextDocument* doc)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TextDocumentPlugin* plugin = qobject_cast<TextDocumentPlugin*>(instance);
        if (plugin)
            plugin->cleanupDocument(doc);
    }
}

void PluginLoader::initTree(TreeWidget* tree)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreeWidgetPlugin* plugin = qobject_cast<TreeWidgetPlugin*>(instance);
        if (plugin)
            plugin->initTree(tree);
    }
}

void PluginLoader::cleanupTree(TreeWidget* tree)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreeWidgetPlugin* plugin = qobject_cast<TreeWidgetPlugin*>(instance);
        if (plugin)
            plugin->cleanupTree(tree);
    }
}
