/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QSplitter>
#include "themeinfo.h"

class IrcBuffer;
class SplitView;
class TreeWidget;
class BufferView;
class TextDocument;
class IrcConnection;
class IrcCommandParser;

class ChatPage : public QSplitter
{
    Q_OBJECT

public:
    ChatPage(QWidget* parent = 0);
    ~ChatPage();

    void init();
    void cleanup();

    IrcBuffer* currentBuffer() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

public slots:
    void closeBuffer(IrcBuffer* buffer = 0);

signals:
    void currentBufferChanged(IrcBuffer* buffer);

private slots:
    void initConnection(IrcConnection* connection);
    void cleanupConnection(IrcConnection* connection);
    void initView(BufferView* view);
    void cleanupView(BufferView* view);
    void initBuffer(IrcBuffer* buffer);
    void cleanupBuffer(IrcBuffer* buffer);
    void initDocument(TextDocument* document);
    void onSocketError();

private:
    static IrcCommandParser* createParser(QObject* parent);

    struct Private {
        ThemeInfo theme;
        SplitView* splitView;
        TreeWidget* treeWidget;
    } d;
};

#endif // CHATPAGE_H
