/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "chatpage.h"
#include "treeview.h"
#include "splitview.h"
#include "document.h"
#include <IrcConnection>
#include <IrcBufferModel>
#include <IrcBuffer>

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.treeView = new TreeView(this);
    d.splitView = new SplitView(this);

    connect(d.treeView, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeView, SLOT(setCurrentBuffer(IrcBuffer*)));

    addWidget(d.treeView);
    addWidget(d.splitView);
    setStretchFactor(1, 1);
    setHandleWidth(1);
}

ChatPage::~ChatPage()
{
}

IrcBuffer* ChatPage::currentBuffer() const
{
    return d.treeView->currentBuffer();
}

QList<IrcConnection*> ChatPage::connections() const
{
    return d.connections;
}

void ChatPage::addConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = new IrcBufferModel(connection);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));
    connect(bufferModel, SIGNAL(messageIgnored(IrcMessage*)), serverBuffer, SLOT(receiveMessage(IrcMessage*)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), Document::instance(), SLOT(addBuffer(IrcBuffer*)));
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), Document::instance(), SLOT(removeBuffer(IrcBuffer*)));

    d.treeView->addModel(bufferModel);
    if (!d.treeView->currentBuffer())
        d.treeView->setCurrentBuffer(serverBuffer);

    d.connections += connection;
}

void ChatPage::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), Document::instance(), SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), Document::instance(), SLOT(removeBuffer(IrcBuffer*)));

    d.treeView->removeModel(bufferModel);
    d.connections.removeOne(connection);
}
