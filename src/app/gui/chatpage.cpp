/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "chatpage.h"
#include "treewidgetex.h"
#include "splitview.h"
#include "document.h"
#include <IrcConnection>
#include <IrcBufferModel>
#include <IrcBuffer>

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.splitView = new SplitView(this);
    d.treeWidget = new TreeWidgetEx(this);

    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeWidget, SLOT(setCurrentBuffer(IrcBuffer*)));

    addWidget(d.treeWidget);
    addWidget(d.splitView);
    setStretchFactor(1, 1);
    setHandleWidth(1);
}

ChatPage::~ChatPage()
{
}

IrcBuffer* ChatPage::currentBuffer() const
{
    return d.treeWidget->currentBuffer();
}

QList<IrcConnection*> ChatPage::connections() const
{
    return d.connections;
}

void ChatPage::addConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = new IrcBufferModel(connection);
    bufferModel->setSortMethod(Irc::SortByTitle);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    serverBuffer->setSticky(true);
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));
    connect(bufferModel, SIGNAL(messageIgnored(IrcMessage*)), serverBuffer, SLOT(receiveMessage(IrcMessage*)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), Document::instance(), SLOT(addBuffer(IrcBuffer*)));
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), Document::instance(), SLOT(removeBuffer(IrcBuffer*)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), d.treeWidget, SLOT(addBuffer(IrcBuffer*)));
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), d.treeWidget, SLOT(removeBuffer(IrcBuffer*)));

    Document::addBuffer(serverBuffer);
    d.treeWidget->addBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

    if (!connection->isActive() && connection->isEnabled())
        connection->open();

    d.connections += connection;
}

void ChatPage::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), Document::instance(), SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), Document::instance(), SLOT(removeBuffer(IrcBuffer*)));

    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), d.treeWidget, SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), d.treeWidget, SLOT(removeBuffer(IrcBuffer*)));

    d.treeWidget->removeBuffer(bufferModel->get(0));
    d.connections.removeOne(connection);
}
