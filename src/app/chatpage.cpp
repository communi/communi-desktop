/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "chatpage.h"
#include "treeitem.h"
#include "treewidget.h"
#include "themeloader.h"
#include "textdocument.h"
#include "pluginloader.h"
#include "textbrowser.h"
#include "bufferview.h"
#include "textinput.h"
#include "splitview.h"
#include "listview.h"
#include "titlebar.h"
#include "overlay.h"
#include "finder.h"
#include "mainwindow.h"
#include "messagehandler.h"
#include <QCoreApplication>
#include <IrcCommandParser>
#include <IrcBufferModel>
#include <QStyleFactory>
#include <IrcConnection>
#include <QStringList>
#include <QProxyStyle>
#include <QScrollBar>
#include <IrcChannel>
#include <IrcBuffer>
#include <QSettings>
#include <Irc>

class ProxyStyle : public QProxyStyle
{
public:
    static ProxyStyle* instance()
    {
        static ProxyStyle style;
        return &style;
    }

    int styleHint(StyleHint hint, const QStyleOption *option = 0,
                  const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
    {
        if (hint == QStyle::SH_ScrollBar_Transient)
            return 1;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }

private:
    ProxyStyle() : QProxyStyle(QStyleFactory::create("fusion")) { }
};

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.finder = new Finder(this);
    d.splitView = new SplitView(this);
    d.treeWidget = new TreeWidget(this);
    addWidget(d.treeWidget);
    addWidget(d.splitView);

#if QT_VERSION >= 0x050300 && !defined(Q_OS_MAC)
    d.treeWidget->horizontalScrollBar()->setStyle(ProxyStyle::instance());
    d.treeWidget->verticalScrollBar()->setStyle(ProxyStyle::instance());
#endif

    connect(d.treeWidget, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));

    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeWidget, SLOT(setCurrentBuffer(IrcBuffer*)));

    connect(d.splitView, SIGNAL(viewAdded(BufferView*)), this, SLOT(addView(BufferView*)));
    connect(d.splitView, SIGNAL(viewRemoved(BufferView*)), this, SLOT(removeView(BufferView*)));
    connect(d.splitView, SIGNAL(currentViewChanged(BufferView*,BufferView*)), this, SLOT(onCurrentViewChanged(BufferView*,BufferView*)));

    setStretchFactor(1, 1);

    addView(d.splitView->currentView());
}

ChatPage::~ChatPage()
{
}

TreeWidget* ChatPage::treeWidget() const
{
    return d.treeWidget;
}

SplitView* ChatPage::splitView() const
{
    return d.splitView;
}

BufferView* ChatPage::currentView() const
{
    return d.splitView->currentView();
}

IrcBuffer* ChatPage::currentBuffer() const
{
    return d.treeWidget->currentBuffer();
}

QString ChatPage::theme() const
{
    return d.theme.name();
}

void ChatPage::setTheme(const QString& theme)
{
    if (!d.theme.isValid() || d.theme.name() != theme) {
        d.theme = ThemeLoader::instance()->theme(theme);
        foreach (TextDocument* doc, d.documents)
            setupDocument(doc);
        foreach (BufferView* view, d.splitView->views())
            view->titleBar()->setStyleSheet(d.theme.style());
        window()->setStyleSheet(d.theme.style());
    }
}

QByteArray ChatPage::saveSettings() const
{
    QVariantMap settings;
    settings.insert("theme", d.theme.name());
    settings.insert("timestamp", d.timestamp);

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << settings;
    return data;
}

void ChatPage::restoreSettings(const QByteArray& data)
{
    QVariantMap settings;
    QDataStream in(data);
    in >> settings;

    d.timestamp = settings.value("timestamp", "[hh:mm:ss]").toString();
    setTheme(settings.value("theme", "Cute").toString());
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

    // restore server buffers
    QList<IrcConnection*> connections = findChildren<IrcConnection*>();
    foreach (IrcConnection* connection, connections) {
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
        if (model) {
            foreach (IrcBuffer* buffer, model->buffers())
                d.splitView->addBuffer(buffer);
        }
    }
}

bool ChatPage::commandFilter(IrcCommand* command)
{
    if (command->type() == IrcCommand::Join) {
        if (command->property("TextInput").toBool())
            d.chans += command->toString().split(" ", QString::SkipEmptyParts).value(1);
    } else if (command->type() == IrcCommand::Custom) {
        const QString cmd = command->parameters().value(0);
        const QStringList params = command->parameters().mid(1);
        if (cmd == "CLEAR") {
            d.splitView->currentView()->textDocument()->clear();
            return true;
        } else if (cmd == "CLOSE") {
            IrcBuffer* buffer = currentBuffer();
            IrcChannel* channel = buffer->toChannel();
            if (channel)
                channel->part(qApp->property("description").toString());
            buffer->deleteLater();
            return true;
        } else if (cmd == "MSG") {
            const QString target = params.value(0);
            const QString message = QStringList(params.mid(1)).join(" ");
            if (!message.isEmpty()) {
                IrcBuffer* buffer = currentBuffer()->model()->add(target);
                IrcCommand* command = IrcCommand::createMessage(target, message);
                if (buffer->sendCommand(command)) {
                    IrcConnection* connection = buffer->connection();
                    IrcMessage* msg = command->toMessage(connection->nickName(), connection);
                    if (msg) {
                        buffer->receiveMessage(msg);
                        msg->deleteLater();
                    }
                }
                d.splitView->currentView()->textInput()->clear();
                d.splitView->setCurrentBuffer(buffer);
                return true;
            }
        } else if (cmd == "QUERY") {
            const QString target = params.value(0);
            const QString message = QStringList(params.mid(1)).join(" ");
            IrcBuffer* buffer = currentBuffer()->model()->add(target);
            if (!message.isEmpty()) {
                IrcCommand* command = IrcCommand::createMessage(target, message);
                if (buffer->sendCommand(command)) {
                    IrcConnection* connection = buffer->connection();
                    IrcMessage* msg = command->toMessage(connection->nickName(), connection);
                    if (msg) {
                        buffer->receiveMessage(msg);
                        msg->deleteLater();
                    }
                }
            }
            d.splitView->currentView()->textInput()->clear();
            d.splitView->setCurrentBuffer(buffer);
            return true;
        } else if (cmd == "SET") {
            const QString key = params.value(0);
            const QString value = params.value(1);
            if (!key.compare("timestamp", Qt::CaseInsensitive)) {
                if (d.timestamp != value) {
                    d.timestamp = value;
                    foreach (TextDocument* doc, d.documents)
                        doc->setTimeStampFormat(value);
                }
            }
            return true;
        }
    }
    return false;
}

void ChatPage::addConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = new IrcBufferModel(connection);
    bufferModel->setSortMethod(Irc::SortByTitle);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    serverBuffer->setSticky(true);
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));
    connect(connection, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));

    MessageHandler* handler = new MessageHandler(bufferModel);
    handler->setDefaultBuffer(serverBuffer);
    handler->setCurrentBuffer(serverBuffer);
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), handler, SLOT(setCurrentBuffer(IrcBuffer*)));

    addBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

    connection->installCommandFilter(this);
    if (!connection->isActive() && connection->isEnabled())
        connection->open();

    PluginLoader::instance()->connectionAdded(connection);
}

void ChatPage::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));

    if (connection->isActive()) {
        connection->quit(qApp->property("description").toString());
        connection->close();
    }
    connection->deleteLater();

    PluginLoader::instance()->connectionRemoved(connection);
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

void ChatPage::addBuffer(IrcBuffer* buffer)
{
    TextDocument* doc = new TextDocument(buffer);
    buffer->setPersistent(true);

    d.treeWidget->addBuffer(buffer);
    d.splitView->addBuffer(buffer);

    PluginLoader::instance()->bufferAdded(buffer);

    setupDocument(doc);
    PluginLoader::instance()->documentAdded(doc);

    connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(removeBuffer(IrcBuffer*)));

    if (buffer->isChannel() && d.chans.contains(buffer->title())) {
        d.chans.removeAll(buffer->title());
        d.splitView->setCurrentBuffer(buffer);
    }
}

void ChatPage::removeBuffer(IrcBuffer* buffer)
{
    QList<TextDocument*> documents = buffer->findChildren<TextDocument*>();
    foreach (TextDocument* doc, documents) {
        d.documents.remove(doc);
        PluginLoader::instance()->documentRemoved(doc);
    }

    d.treeWidget->removeBuffer(buffer);
    d.splitView->removeBuffer(buffer);

    if (buffer->isSticky())
        buffer->connection()->deleteLater();

    PluginLoader::instance()->bufferRemoved(buffer);
}

void ChatPage::setupDocument(TextDocument* document)
{
    d.documents.insert(document);

    document->setTimeStampFormat(d.timestamp);
    document->setStyleSheet(d.theme.style());

    if (!document->isClone()) {
        connect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SIGNAL(messageHighlighted(IrcMessage*)));
        connect(document, SIGNAL(privateMessageReceived(IrcMessage*)), this, SIGNAL(privateMessageReceived(IrcMessage*)));
    }
}

void ChatPage::addView(BufferView* view)
{
    TitleBar* bar = view->titleBar();
    bar->setStyleSheet(d.theme.style());

#if QT_VERSION >= 0x050300 && !defined(Q_OS_MAC)
    view->textBrowser()->horizontalScrollBar()->setStyle(ProxyStyle::instance());
    view->textBrowser()->verticalScrollBar()->setStyle(ProxyStyle::instance());
    view->listView()->horizontalScrollBar()->setStyle(ProxyStyle::instance());
    view->listView()->verticalScrollBar()->setStyle(ProxyStyle::instance());
#endif

    view->textInput()->setParser(createParser(view));
    connect(view, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));
    connect(view, SIGNAL(cloned(TextDocument*)), this, SLOT(setupDocument(TextDocument*)));
    connect(view, SIGNAL(cloned(TextDocument*)), PluginLoader::instance(), SLOT(documentAdded(TextDocument*)));

    new Overlay(view);

    PluginLoader::instance()->viewAdded(view);
}

void ChatPage::removeView(BufferView* view)
{
    PluginLoader::instance()->viewRemoved(view);
}

void ChatPage::onCurrentViewChanged(BufferView* current, BufferView* previous)
{
    d.finder->cancelListSearch(previous);
    d.finder->cancelBrowserSearch(previous);
    emit currentViewChanged(current);
}

void ChatPage::onSocketError()
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection) {
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>(); // TODO
        if (model) {
            IrcBuffer* buffer = model->get(0);
            if (buffer) {
                QStringList params = QStringList() << connection->nickName() << connection->socket()->errorString();
                IrcMessage* message = IrcMessage::fromParameters(buffer->title(), QString::number(Irc::ERR_UNKNOWNERROR), params, connection);
                foreach (TextDocument* doc, buffer->findChildren<TextDocument*>())
                    doc->receiveMessage(message);
                delete message;

                TreeItem* item = d.treeWidget->connectionItem(connection);
                if (item && d.treeWidget->currentItem() != item)
                    d.treeWidget->highlightItem(item);
            }
        }
    }
}

IrcCommandParser* ChatPage::createParser(QObject *parent)
{
    IrcCommandParser* parser = new IrcCommandParser(parent);
    parser->setTriggers(QStringList("/"));
    parser->setTolerant(true);

    // TODO: IrcCommandParser: static default commands?
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

    parser->addCommand(IrcCommand::Custom, "CLEAR");
    parser->addCommand(IrcCommand::Custom, "CLOSE");
    parser->addCommand(IrcCommand::Custom, "MSG <user/channel> <message...>");
    parser->addCommand(IrcCommand::Custom, "QUERY <user> (<message...>)");
    parser->addCommand(IrcCommand::Custom, "SET <key> (<value>)");

    return parser;
}
