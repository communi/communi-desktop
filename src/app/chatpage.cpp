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
#include "bufferview.h"
#include "textinput.h"
#include "splitview.h"
#include "mainwindow.h"
#include <QCoreApplication>
#include <IrcCommandParser>
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcChannel>
#include <IrcBuffer>

#include <QtPlugin>
#include <QPluginLoader>
#include "bufferplugin.h"
#include "bufferviewplugin.h"
#include "connectionplugin.h"
#include "splitviewplugin.h"
#include "textdocumentplugin.h"
#include "treewidgetplugin.h"
#include "mainwindowplugin.h"

Q_IMPORT_PLUGIN(AjaxPlugin)
Q_IMPORT_PLUGIN(BadgePlugin)
Q_IMPORT_PLUGIN(CommanderPlugin)
Q_IMPORT_PLUGIN(CompleterPlugin)
Q_IMPORT_PLUGIN(DockPlugin)
Q_IMPORT_PLUGIN(FinderPlugin)
Q_IMPORT_PLUGIN(HighlighterPlugin)
Q_IMPORT_PLUGIN(HistoryPlugin)
Q_IMPORT_PLUGIN(IgnorePlugin)
Q_IMPORT_PLUGIN(LagPlugin)
Q_IMPORT_PLUGIN(MenuPlugin)
Q_IMPORT_PLUGIN(MonitorPlugin)
Q_IMPORT_PLUGIN(NavigatorPlugin)
Q_IMPORT_PLUGIN(ReseterPlugin)
Q_IMPORT_PLUGIN(SorterPlugin)
Q_IMPORT_PLUGIN(SoundPlugin)
Q_IMPORT_PLUGIN(StatePlugin)
Q_IMPORT_PLUGIN(StylePlugin)
Q_IMPORT_PLUGIN(TopicPlugin)
Q_IMPORT_PLUGIN(TrayPlugin)
Q_IMPORT_PLUGIN(VerifierPlugin)
Q_IMPORT_PLUGIN(ZncPlugin)

static QString applicationDescription()
{
    return ChatPage::tr("%1 %2 - http://%3").arg(QCoreApplication::applicationName())
                                            .arg(QCoreApplication::applicationVersion())
                                            .arg(QCoreApplication::organizationDomain());
}

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.splitView = new SplitView(this);
    d.treeWidget = new TreeWidget(this);
    addWidget(d.treeWidget);
    addWidget(d.splitView);

    connect(d.treeWidget, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));

    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeWidget, SLOT(setCurrentBuffer(IrcBuffer*)));

    connect(d.splitView, SIGNAL(viewAdded(BufferView*)), this, SLOT(initView(BufferView*)));
    connect(d.splitView, SIGNAL(viewRemoved(BufferView*)), this, SLOT(uninitView(BufferView*)));

    setStretchFactor(1, 1);
}

ChatPage::~ChatPage()
{
}

void ChatPage::init()
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        MainWindowPlugin* windowPlugin = qobject_cast<MainWindowPlugin*>(instance);
        if (windowPlugin)
            windowPlugin->initialize(static_cast<MainWindow*>(window()));
        TreeWidgetPlugin* treePlugin = qobject_cast<TreeWidgetPlugin*>(instance);
        if (treePlugin)
            treePlugin->initialize(d.treeWidget);
        SplitViewPlugin* viewPlugin = qobject_cast<SplitViewPlugin*>(instance);
        if (viewPlugin)
            viewPlugin->initialize(d.splitView);
    }
    initView(d.splitView->currentView());
}

void ChatPage::uninit()
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreeWidgetPlugin* treePlugin = qobject_cast<TreeWidgetPlugin*>(instance);
        if (treePlugin)
            treePlugin->uninitialize(d.treeWidget);
        SplitViewPlugin* viewPlugin = qobject_cast<SplitViewPlugin*>(instance);
        if (viewPlugin)
            viewPlugin->uninitialize(d.splitView);
        MainWindowPlugin* windowPlugin = qobject_cast<MainWindowPlugin*>(instance);
        if (windowPlugin)
            windowPlugin->uninitialize(static_cast<MainWindow*>(window()));
    }
}

IrcBuffer* ChatPage::currentBuffer() const
{
    return d.treeWidget->currentBuffer();
}

void ChatPage::initConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = new IrcBufferModel(connection);
    bufferModel->setSortMethod(Irc::SortByTitle);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    serverBuffer->setSticky(true);
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));
    connect(bufferModel, SIGNAL(messageIgnored(IrcMessage*)), serverBuffer, SLOT(receiveMessage(IrcMessage*)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(initBuffer(IrcBuffer*)));
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(uninitBuffer(IrcBuffer*)));

    initBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

    if (!connection->isActive() && connection->isEnabled())
        connection->open();

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ConnectionPlugin* plugin = qobject_cast<ConnectionPlugin*>(instance);
        if (plugin)
            plugin->initialize(connection);
    }
}

void ChatPage::uninitConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(initBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(uninitBuffer(IrcBuffer*)));

    if (connection->isActive()) {
        connection->quit(applicationDescription());
        connection->close();
    }
    connection->deleteLater();

    // TODO:
//    if (d.connections.isEmpty())
//        d.splitView->reset();

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ConnectionPlugin* connectionPlugin = qobject_cast<ConnectionPlugin*>(instance);
        if (connectionPlugin)
            connectionPlugin->uninitialize(connection);
    }
}

void ChatPage::closeBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = d.treeWidget->currentBuffer();

    IrcChannel* channel = buffer->toChannel();
    if (channel && channel->isActive())
        channel->part(applicationDescription());
    buffer->deleteLater();
}

void ChatPage::initBuffer(IrcBuffer* buffer)
{
    TextDocument* doc = new TextDocument(buffer);
    buffer->setPersistent(true);

    d.treeWidget->addBuffer(buffer);

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferPlugin* bufferPlugin = qobject_cast<BufferPlugin*>(instance);
        if (bufferPlugin) {
            bufferPlugin->addBuffer(buffer);
            connect(buffer, SIGNAL(destroyed(IrcBuffer*)), instance, SLOT(removeBuffer(IrcBuffer*)));
            connect(this, SIGNAL(currentBufferChanged(IrcBuffer*)), instance, SLOT(setCurrentBuffer(IrcBuffer*)));
        }
        TextDocumentPlugin* documentPlugin = qobject_cast<TextDocumentPlugin*>(instance);
        if (documentPlugin)
            documentPlugin->initialize(doc);
    }
}

void ChatPage::uninitBuffer(IrcBuffer* buffer)
{
    TextDocument* doc = buffer->property("document").value<TextDocument*>();
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TextDocumentPlugin* plugin = qobject_cast<TextDocumentPlugin*>(instance);
        if (plugin)
            plugin->uninitialize(doc);
    }

    d.treeWidget->removeBuffer(buffer);

    if (buffer->isSticky())
        buffer->connection()->deleteLater();
}

void ChatPage::initView(BufferView* view)
{
    view->textInput()->setParser(createParser(view));

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferViewPlugin* plugin = qobject_cast<BufferViewPlugin*>(instance);
        if (plugin)
            plugin->initialize(view);
    }
}

void ChatPage::uninitView(BufferView* view)
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferViewPlugin* plugin = qobject_cast<BufferViewPlugin*>(instance);
        if (plugin)
            plugin->uninitialize(view);
    }
}

IrcCommandParser* ChatPage::createParser(QObject *parent)
{
    IrcCommandParser* parser = new IrcCommandParser(parent);
    parser->setTriggers(QStringList("/"));
    parser->setTolerant(true);

    parser->addCommand(IrcCommand::CtcpAction, "ACTION <target> <message...>");
    parser->addCommand(IrcCommand::Admin, "ADMIN (<server>)");
    parser->addCommand(IrcCommand::Away, "AWAY (<reason...>)");
    parser->addCommand(IrcCommand::Info, "INFO (<server>)");
    parser->addCommand(IrcCommand::Invite, "INVITE <user> (<#channel>)");
    parser->addCommand(IrcCommand::Join, "JOIN <#channel> (<key>)");
    parser->addCommand(IrcCommand::Kick, "KICK (<#channel>) <user> (<reason...>)");
    parser->addCommand(IrcCommand::Knock, "KNOCK <#channel> (<message...>)");
    parser->addCommand(IrcCommand::List, "LIST (<channels>) (<server>)");
    parser->addCommand(IrcCommand::CtcpAction, "ME [target] <message...>");
    parser->addCommand(IrcCommand::Mode, "MODE (<channel/user>) (<mode>) (<arg>)");
    parser->addCommand(IrcCommand::Motd, "MOTD (<server>)");
    parser->addCommand(IrcCommand::Names, "NAMES (<#channel>)");
    parser->addCommand(IrcCommand::Nick, "NICK <nick>");
    parser->addCommand(IrcCommand::Notice, "NOTICE <#channel/user> <message...>");
    parser->addCommand(IrcCommand::Part, "PART (<#channel>) (<message...>)");
    parser->addCommand(IrcCommand::Ping, "PING (<user>)");
    parser->addCommand(IrcCommand::Quit, "QUIT (<message...>)");
    parser->addCommand(IrcCommand::Quote, "QUOTE <command> (<parameters...>)");
    parser->addCommand(IrcCommand::Stats, "STATS <query> (<server>)");
    parser->addCommand(IrcCommand::Time, "TIME (<user>)");
    parser->addCommand(IrcCommand::Topic, "TOPIC (<#channel>) (<topic...>)");
    parser->addCommand(IrcCommand::Trace, "TRACE (<target>)");
    parser->addCommand(IrcCommand::Users, "USERS (<server>)");
    parser->addCommand(IrcCommand::Version, "VERSION (<user>)");
    parser->addCommand(IrcCommand::Who, "WHO <user>");
    parser->addCommand(IrcCommand::Whois, "WHOIS <user>");
    parser->addCommand(IrcCommand::Whowas, "WHOWAS <user>");

    return parser;
}
