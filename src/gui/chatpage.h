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

class IrcUser;
class TreeView;
class IrcBuffer;
class TextEntry;
class HelpLabel;
class IrcMessage;
class BufferView;
class TopicLabel;
class SearchEntry;
class UserListView;
class IrcConnection;

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    ChatPage(QWidget* parent = 0);
    ~ChatPage();

    IrcBuffer* currentBuffer() const;

    QList<IrcConnection*> connections() const;
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

signals:
    void highlighted(IrcMessage* message);
    void currentBufferChanged(IrcBuffer* buffer);

private slots:
    void searchBuffer();
    void onEscPressed();
    void queryUser(const QString& user);
    void setCurrentBuffer(IrcBuffer* buffer);

private:
    struct Private {
        TreeView* treeView;
        HelpLabel* helpLabel;
        TextEntry* textEntry;
        BufferView* bufferView;
        TopicLabel* topicLabel;
        QSplitter* innerSplitter;
        QSplitter* outerSplitter;
        SearchEntry* searchEntry;
        UserListView* userListView;
        QList<IrcConnection*> connections;
    } d;
};

#endif // CHATPAGE_H
