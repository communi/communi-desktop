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
#include <QCoreApplication>
#include <IrcCommandParser>
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcBuffer>

#include <QtPlugin>
#include <QPluginLoader>
#include "bufferviewplugin.h"
#include "connectionplugin.h"
#include "splitviewplugin.h"
#include "textdocumentplugin.h"
#include "treewidgetplugin.h"
#include "windowplugin.h"

Q_IMPORT_PLUGIN(AjaxPlugin)
Q_IMPORT_PLUGIN(BadgePlugin)
Q_IMPORT_PLUGIN(CommanderPlugin)
Q_IMPORT_PLUGIN(CompleterPlugin)
Q_IMPORT_PLUGIN(DockPlugin)
Q_IMPORT_PLUGIN(FinderPlugin)
Q_IMPORT_PLUGIN(HighlighterPlugin)
Q_IMPORT_PLUGIN(HistoryPlugin)
Q_IMPORT_PLUGIN(IgnorePlugin)
Q_IMPORT_PLUGIN(MenuPlugin)
Q_IMPORT_PLUGIN(MonitorPlugin)
Q_IMPORT_PLUGIN(NavigatorPlugin)
Q_IMPORT_PLUGIN(ReseterPlugin)
Q_IMPORT_PLUGIN(SoundPlugin)
Q_IMPORT_PLUGIN(SubjectPlugin)
Q_IMPORT_PLUGIN(StylePlugin)
Q_IMPORT_PLUGIN(TrayPlugin)
Q_IMPORT_PLUGIN(ZncPlugin)

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

    setStretchFactor(1, 1);

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreeWidgetPlugin* treePlugin = qobject_cast<TreeWidgetPlugin*>(instance);
        if (treePlugin)
            treePlugin->initialize(d.treeWidget);
        SplitViewPlugin* viewPlugin = qobject_cast<SplitViewPlugin*>(instance);
        if (viewPlugin)
            viewPlugin->initialize(d.splitView);
        WindowPlugin* windowPlugin = qobject_cast<WindowPlugin*>(instance);
        if (windowPlugin)
            windowPlugin->initialize(window());
    }

    addView(d.splitView->currentView());
}

ChatPage::~ChatPage()
{
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TreeWidgetPlugin* treePlugin = qobject_cast<TreeWidgetPlugin*>(instance);
        if (treePlugin)
            treePlugin->uninitialize(d.treeWidget);
        SplitViewPlugin* viewPlugin = qobject_cast<SplitViewPlugin*>(instance);
        if (viewPlugin)
            viewPlugin->uninitialize(d.splitView);
        WindowPlugin* windowPlugin = qobject_cast<WindowPlugin*>(instance);
        if (windowPlugin)
            windowPlugin->uninitialize(window());
    }
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

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ConnectionPlugin* plugin = qobject_cast<ConnectionPlugin*>(instance);
        if (plugin)
            plugin->initialize(connection);
    }
}

void ChatPage::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(removeBuffer(IrcBuffer*)));
    d.connections.removeOne(connection);

    if (connection->isActive()) {
        QString reason = tr("%1 %2 - http://%3").arg(QCoreApplication::applicationName())
                                                .arg(QCoreApplication::applicationVersion())
                                                .arg(QCoreApplication::organizationDomain());
        connection->quit(reason);
        connection->close();
    }
    connection->deleteLater();

    if (d.connections.isEmpty())
        d.splitView->reset();

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        ConnectionPlugin* connectionPlugin = qobject_cast<ConnectionPlugin*>(instance);
        if (connectionPlugin)
            connectionPlugin->uninitialize(connection);
    }
}

void ChatPage::addBuffer(IrcBuffer* buffer)
{
    d.treeWidget->addBuffer(buffer);
    TextDocument* doc = new TextDocument(buffer);
    buffer->setProperty("document", QVariant::fromValue(doc));
    buffer->setPersistent(true);

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TextDocumentPlugin* plugin = qobject_cast<TextDocumentPlugin*>(instance);
        if (plugin)
            plugin->initialize(doc);
    }
}

void ChatPage::removeBuffer(IrcBuffer* buffer)
{
    if (buffer->isSticky())
        removeConnection(buffer->connection());

    TextDocument* doc = buffer->property("document").value<TextDocument*>();
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TextDocumentPlugin* plugin = qobject_cast<TextDocumentPlugin*>(instance);
        if (plugin)
            plugin->uninitialize(doc);
    }

    d.treeWidget->removeBuffer(buffer);
}

void ChatPage::addView(BufferView* view)
{
    view->textInput()->setParser(createParser(view));

    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BufferViewPlugin* plugin = qobject_cast<BufferViewPlugin*>(instance);
        if (plugin)
            plugin->initialize(view);
    }
}

void ChatPage::removeView(BufferView* view)
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
