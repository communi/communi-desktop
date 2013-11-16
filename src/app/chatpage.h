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

class IrcBuffer;
class SplitView;
class TreeWidget;
class BufferView;
class IrcConnection;
class IrcCommandParser;

class ChatPage : public QSplitter
{
    Q_OBJECT

public:
    ChatPage(QWidget* parent = 0);
    ~ChatPage();

    void init();
    void uninit();

    IrcBuffer* currentBuffer() const;

public slots:
    void closeBuffer(IrcBuffer* buffer = 0);

signals:
    void currentBufferChanged(IrcBuffer* buffer);

private slots:
    void initConnection(IrcConnection* connection);
    void uninitConnection(IrcConnection* connection);
    void initView(BufferView* view);
    void uninitView(BufferView* view);
    void initBuffer(IrcBuffer* buffer);
    void uninitBuffer(IrcBuffer* buffer);

private:
    static IrcCommandParser* createParser(QObject* parent);

    struct Private {
        SplitView* splitView;
        TreeWidget* treeWidget;
    } d;
};

#endif // CHATPAGE_H
