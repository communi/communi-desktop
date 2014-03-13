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

#include <QObject>

class IrcBuffer;
class TreeWidget;
class BufferView;
class TextDocument;
class IrcConnection;

class PluginLoader : public QObject
{
    Q_OBJECT

public:
    static PluginLoader* instance();

public slots:
    void initBuffer(IrcBuffer* buffer);
    void cleanupBuffer(IrcBuffer* buffer);

    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);

    void initView(BufferView* view);
    void cleanupView(BufferView* view);

    void initDocument(TextDocument* doc);
    void cleanupDocument(TextDocument* doc);

    void initTree(TreeWidget* tree);
    void cleanupTree(TreeWidget* tree);

private:
    PluginLoader(QObject* parent = 0);
};

#endif // CHATPAGE_H
