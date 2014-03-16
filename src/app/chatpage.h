/*
 * Copyright (C) 2008-2014 The Communi Project
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

    QString theme() const;
    void setTheme(const QString& theme);

    TreeWidget* treeWidget() const;
    SplitView* splitView() const;

    BufferView* currentView() const;
    IrcBuffer* currentBuffer() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

public slots:
    void addBuffer(IrcBuffer* buffer);
    void closeBuffer(IrcBuffer* buffer = 0);
    void removeBuffer(IrcBuffer* buffer);

signals:
    void currentViewChanged(BufferView* view);
    void currentBufferChanged(IrcBuffer* buffer);

private slots:
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);
    void addView(BufferView* view);
    void removeView(BufferView* view);
    void addDocument(TextDocument* document);
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
