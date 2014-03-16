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
#include "bufferview.h"
#include "textinput.h"
#include "splitview.h"
#include "titlebar.h"
#include "finder.h"
#include "mainwindow.h"
#include "messageformatter.h"
#include <QCoreApplication>
#include <IrcCommandParser>
#include <IrcBufferModel>
#include <IrcConnection>
#include <QStringList>
#include <IrcChannel>
#include <IrcBuffer>
#include <QSettings>
#include <Irc>

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.showEvents = true;
    d.showDetails = true;
    d.timestampFormat = "[hh:mm:ss]";

    d.splitView = new SplitView(this);
    d.treeWidget = new TreeWidget(this);
    addWidget(d.treeWidget);
    addWidget(d.splitView);

    connect(d.treeWidget, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));

    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeWidget, SLOT(setCurrentBuffer(IrcBuffer*)));

    connect(d.splitView, SIGNAL(viewAdded(BufferView*)), this, SLOT(addView(BufferView*)));
    connect(d.splitView, SIGNAL(viewRemoved(BufferView*)), this, SLOT(removeView(BufferView*)));
    connect(d.splitView, SIGNAL(currentViewChanged(BufferView*)), this, SIGNAL(currentViewChanged(BufferView*)));

    setStretchFactor(1, 1);
}

ChatPage::~ChatPage()
{
}

void ChatPage::init()
{
    addView(d.splitView->currentView());
    new Finder(this);
}

void ChatPage::cleanup()
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

        QString css = d.theme.style();
        window()->setStyleSheet(css);

        QTreeWidgetItemIterator it(d.treeWidget);
        while (*it) {
            TreeItem* item = static_cast<TreeItem*>(*it);
            QList<TextDocument*> documents = item->buffer()->findChildren<TextDocument*>();
            foreach (TextDocument* doc, documents)
                doc->setStyleSheet(css);
            it++;
        }
    }
}

QFont ChatPage::messageFont() const
{
    return d.messageFont;
}

void ChatPage::setMessageFont(const QFont& font)
{
    d.messageFont = font;
}

QString ChatPage::timeStampFormat() const
{
    return d.timestampFormat;
}

void ChatPage::setTimeStampFormat(const QString& format)
{
    d.timestampFormat = format;
}

bool ChatPage::showEvents() const
{
    return d.showEvents;
}

void ChatPage::setShowEvents(bool show)
{
    d.showEvents = show;
}

bool ChatPage::showDetails() const
{
    return d.showDetails;
}

void ChatPage::setShowDetails(bool show)
{
    d.showDetails = show;
}

QByteArray ChatPage::saveSettings() const
{
    QVariantMap settings;
    settings.insert("theme", d.theme.name());
    settings.insert("showEvents", d.showEvents);
    settings.insert("showDetails", d.showDetails);
    settings.insert("fontFamily", d.messageFont.family());
    settings.insert("fontSize", d.messageFont.pixelSize());

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

    setTheme(settings.value("theme", "Cute").toString());
    setShowEvents(settings.value("showEvents", true).toBool());
    setShowDetails(settings.value("showDetails", true).toBool());

    QFont font;
    font.setFamily(settings.value("fontFamily", font.family()).toString());
    font.setPixelSize(settings.value("fontSize", font.pixelSize()).toInt());
    setMessageFont(font);
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

void ChatPage::addConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = new IrcBufferModel(connection);
    bufferModel->setSortMethod(Irc::SortByTitle);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    serverBuffer->setSticky(true);
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));
    // TODO: more fine-grained delivery (WHOIS replies etc. to the current buffer)
    connect(bufferModel, SIGNAL(messageIgnored(IrcMessage*)), serverBuffer, SLOT(receiveMessage(IrcMessage*)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));
    connect(connection, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));

    addBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

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
}

void ChatPage::removeBuffer(IrcBuffer* buffer)
{
    QList<TextDocument*> documents = buffer->findChildren<TextDocument*>();
    foreach (TextDocument* doc, documents)
        PluginLoader::instance()->documentRemoved(doc);

    d.treeWidget->removeBuffer(buffer);
    d.splitView->removeBuffer(buffer);

    if (buffer->isSticky())
        buffer->connection()->deleteLater();

    PluginLoader::instance()->bufferRemoved(buffer);
}

void ChatPage::setupDocument(TextDocument* document)
{
    document->setStyleSheet(d.theme.style());
    document->setDefaultFont(d.messageFont);
    document->formatter()->setDetailed(d.showDetails);
    document->formatter()->setStripNicks(!d.showDetails);
    document->formatter()->setTimeStampFormat(d.timestampFormat);
}

void ChatPage::addView(BufferView* view)
{
    TitleBar* bar = view->titleBar();
    QTextDocument* doc = bar->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet(d.theme.style());

    view->textInput()->setParser(createParser(view));
    connect(view, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));
    connect(view, SIGNAL(cloned(TextDocument*)), this, SLOT(setupDocument(TextDocument*)));
    connect(view, SIGNAL(cloned(TextDocument*)), PluginLoader::instance(), SLOT(documentAdded(TextDocument*)));

    PluginLoader::instance()->viewAdded(view);
}

void ChatPage::removeView(BufferView* view)
{
    PluginLoader::instance()->viewRemoved(view);
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

    return parser;
}
