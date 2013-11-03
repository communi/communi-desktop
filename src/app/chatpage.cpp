/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "chatpage.h"
#include "treewidget.h"
#include "textdocument.h"
#include "commandparser.h"
#include "bufferview.h"
#include "textinput.h"
#include "splitview.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcBuffer>

#include <QPluginLoader>
#include "browserplugin.h"
#include "documentplugin.h"
#include "listplugin.h"
#include "inputplugin.h"
#include "topicplugin.h"
#include "treeplugin.h"
#include "viewplugin.h"

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.splitView = new SplitView(this);
    d.treeWidget = new TreeWidget(this);
    addWidget(d.treeWidget);
    addWidget(d.splitView);

    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeWidget, SLOT(setCurrentBuffer(IrcBuffer*)));

    connect(d.splitView, SIGNAL(viewAdded(BufferView*)), this, SLOT(addView(BufferView*)));
    connect(d.splitView, SIGNAL(viewRemoved(BufferView*)), this, SLOT(removeView(BufferView*)));
    addView(d.splitView->currentView());

    setStretchFactor(1, 1);
    setHandleWidth(1);

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreePlugin* treePlugin = qobject_cast<TreePlugin*>(instance);
        if (treePlugin)
            treePlugin->initialize(d.treeWidget);
        ViewPlugin* viewPlugin = qobject_cast<ViewPlugin*>(instance);
        if (viewPlugin)
            viewPlugin->initialize(d.splitView);
    }
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

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(removeBuffer(IrcBuffer*)));

    addBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

    if (!connection->isActive() && connection->isEnabled())
        connection->open();

    d.connections += connection;
}

void ChatPage::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();

    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(removeBuffer(IrcBuffer*)));

    removeBuffer(bufferModel->get(0));
    d.connections.removeOne(connection);
}

void ChatPage::addBuffer(IrcBuffer* buffer)
{
    d.treeWidget->addBuffer(buffer);
    TextDocument* doc = new TextDocument(buffer);
    buffer->setProperty("document", QVariant::fromValue(doc));

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        DocumentPlugin* plugin = qobject_cast<DocumentPlugin*>(instance);
        if (plugin)
            plugin->initialize(doc);
    }
}

void ChatPage::removeBuffer(IrcBuffer* buffer)
{
    d.treeWidget->removeBuffer(buffer);
}

void ChatPage::addView(BufferView* view)
{
    CommandParser* parser = new CommandParser(view);
    parser->setTriggers(QStringList("/"));
    parser->setTolerant(true);
    view->textInput()->setParser(parser);

    // TODO: move outta here...?
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BrowserPlugin* browserPlugin = qobject_cast<BrowserPlugin*>(instance);
        if (browserPlugin)
            browserPlugin->initialize(view->textBrowser());
        InputPlugin* inputPlugin = qobject_cast<InputPlugin*>(instance);
        if (inputPlugin)
            inputPlugin->initialize(view->textInput());
        ListPlugin* listPlugin = qobject_cast<ListPlugin*>(instance);
        if (listPlugin)
            listPlugin->initialize(view->listView());
        TopicPlugin* topicPlugin = qobject_cast<TopicPlugin*>(instance);
        if (topicPlugin)
            topicPlugin->initialize(view->topicLabel());
    }
}

void ChatPage::removeView(BufferView* view)
{
    // TODO
}
