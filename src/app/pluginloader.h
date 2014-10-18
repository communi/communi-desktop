/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QPluginLoader>

class IrcBuffer;
class ThemeInfo;
class TreeWidget;
class BufferView;
class TextDocument;
class IrcConnection;

class PluginLoader : public QObject
{
    Q_OBJECT

public:
    static PluginLoader* instance();

    static QStringList paths();

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

private:
    PluginLoader(QObject* parent = 0);
};

#endif // CHATPAGE_H
