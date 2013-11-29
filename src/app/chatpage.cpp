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
#include "pluginloader.h"
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
#include <QSettings>

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
    connect(d.splitView, SIGNAL(viewRemoved(BufferView*)), this, SLOT(cleanupView(BufferView*)));

    setStretchFactor(1, 1);
}

ChatPage::~ChatPage()
{
}

void ChatPage::init()
{
    PluginLoader::instance()->initTree(d.treeWidget);
    PluginLoader::instance()->initView(d.splitView);
    initView(d.splitView->currentView());
}

void ChatPage::cleanup()
{
    PluginLoader::instance()->cleanupTree(d.treeWidget);
    PluginLoader::instance()->cleanupView(d.splitView);
}

IrcBuffer* ChatPage::currentBuffer() const
{
    return d.treeWidget->currentBuffer();
}

QByteArray ChatPage::saveState() const
{
    QVariantMap state;
    state.insert("splitter", QSplitter::saveState());
    state.insert("views", d.splitView->saveState());
    state.insert("tree", d.treeWidget->saveState());

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << state;
    return data;
}

void ChatPage::restoreState(const QByteArray& data)
{
    QVariantMap state;
    QDataStream in(data);
    in >> state;

    if (state.contains("tree"))
        d.treeWidget->restoreState(state.value("tree").toByteArray());
    if (state.contains("splitter"))
        QSplitter::restoreState(state.value("splitter").toByteArray());
    if (state.contains("views"))
        d.splitView->restoreState(state.value("views").toByteArray());
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
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(cleanupBuffer(IrcBuffer*)));

    initBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

    if (!connection->isActive() && connection->isEnabled())
        connection->open();

    PluginLoader::instance()->initConnection(connection);
}

void ChatPage::cleanupConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(initBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), this, SLOT(cleanupBuffer(IrcBuffer*)));

    if (connection->isActive()) {
        connection->quit(qApp->property("description").toString());
        connection->close();
    }
    connection->deleteLater();

    PluginLoader::instance()->cleanupConnection(connection);
}

void ChatPage::closeBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = d.treeWidget->currentBuffer();

    IrcChannel* channel = buffer->toChannel();
    if (channel && channel->isActive())
        channel->part(qApp->property("description").toString());
    buffer->deleteLater();
}

void ChatPage::initBuffer(IrcBuffer* buffer)
{
    TextDocument* doc = new TextDocument(buffer);
    buffer->setPersistent(true);

    d.treeWidget->addBuffer(buffer);
    d.splitView->initBuffer(buffer);

    PluginLoader::instance()->initBuffer(buffer);
    PluginLoader::instance()->initDocument(doc);
}

void ChatPage::cleanupBuffer(IrcBuffer* buffer)
{
    QList<TextDocument*> documents = buffer->findChildren<TextDocument*>();
    foreach (TextDocument* doc, documents)
        PluginLoader::instance()->cleanupDocument(doc);

    d.treeWidget->removeBuffer(buffer);
    d.splitView->cleanupBuffer(buffer);

    if (buffer->isSticky())
        buffer->connection()->deleteLater();

    PluginLoader::instance()->cleanupBuffer(buffer);
}

void ChatPage::initView(BufferView* view)
{
    view->textInput()->setParser(createParser(view));
    connect(view, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));
    connect(view, SIGNAL(cloned(TextDocument*)), PluginLoader::instance(), SLOT(initDocument(TextDocument*)));

    PluginLoader::instance()->initView(view);
}

void ChatPage::cleanupView(BufferView* view)
{
    PluginLoader::instance()->cleanupView(view);
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
