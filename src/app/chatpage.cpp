/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "chatpage.h"
#include "treeitem.h"
#include "treerole.h"
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
#include "scrollbarstyle.h"
#include "messagehandler.h"
#include <QCoreApplication>
#include <IrcCommandParser>
#include <IrcBufferModel>
#include <IrcConnection>
#include <QStringList>
#include <QScrollBar>
#include <IrcChannel>
#include <IrcBuffer>
#include <QSettings>
#include <Irc>

ChatPage::ChatPage(QWidget* parent) : QSplitter(parent)
{
    d.finder = new Finder(this);
    d.splitView = new SplitView(this);
    d.treeWidget = new TreeWidget(this);
    addWidget(d.treeWidget);
    addWidget(d.splitView);

#if QT_VERSION >= 0x050300 && !defined(Q_OS_MAC)
    d.treeWidget->verticalScrollBar()->setStyle(ScrollBarStyle::expanding());
#endif

    connect(d.treeWidget, SIGNAL(bufferClosed(IrcBuffer*)), this, SLOT(closeBuffer(IrcBuffer*)));

    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(d.treeWidget, SIGNAL(currentBufferChanged(IrcBuffer*)), d.splitView, SLOT(setCurrentBuffer(IrcBuffer*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), d.treeWidget, SLOT(setCurrentBuffer(IrcBuffer*)));

    connect(d.splitView, SIGNAL(viewAdded(BufferView*)), this, SLOT(addView(BufferView*)));
    connect(d.splitView, SIGNAL(viewRemoved(BufferView*)), this, SLOT(removeView(BufferView*)));
    connect(d.splitView, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(onCurrentBufferChanged(IrcBuffer*)));
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

        // TODO: because of theme preview
        if (window()->inherits("QMainWindow"))
            PluginLoader::instance()->themeChanged(d.theme);
    }
}

QByteArray ChatPage::saveSettings() const
{
    QVariantMap settings;
    settings.insert("theme", d.theme.name());
    settings.insert("timestamp", d.timestamp);
    settings.insert("tree", d.treeWidget->saveState());

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

    if (settings.contains("tree"))
        d.treeWidget->restoreState(settings.value("tree").toByteArray());

    d.timestamp = settings.value("timestamp", "[hh:mm:ss]").toString();
    setTheme(settings.value("theme", "Cute").toString());
}

QByteArray ChatPage::saveState() const
{
    QVariantMap state;
    state.insert("splitter", QSplitter::saveState());
    state.insert("views", d.splitView->saveState());

    QVariantMap timestamps;
    foreach (TextDocument* doc, d.documents) {
        if (doc->timestamp().isValid()) {
            IrcBuffer* buffer = doc->buffer();
            QString id = buffer->connection()->userData().value("uuid").toString();
            id += "/" + buffer->title();
            timestamps[id] = doc->timestamp();
        }
    }
    state.insert("timestamps", timestamps);

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

    d.timestamps = state.value("timestamps").toMap();

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
            d.splitView->currentView()->textBrowser()->clear();
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
                IrcCommand* command = IrcCommand::createMessage(target, message);
                IrcConnection* connection = currentBuffer()->connection();
                if (connection->sendCommand(command)) {
                    IrcMessage* msg = command->toMessage(connection->nickName(), connection);
                    if (msg) {
                        IrcBuffer* buffer = currentBuffer()->model()->add(msg->property("target").toString());
                        d.splitView->setCurrentBuffer(buffer);
                        buffer->receiveMessage(msg);
                        msg->deleteLater();
                    }
                }
                d.splitView->currentView()->textInput()->clear();
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
            const QString key = params.value(0).toLower();
            const QString value = QStringList(params.mid(1)).join(" ");
            if (!key.compare("timestamp", Qt::CaseInsensitive)) {
                if (d.timestamp != value) {
                    d.timestamp = value;
                    foreach (TextDocument* doc, d.documents)
                        doc->setTimeStampFormat(value);
                }
            } else if (!key.compare("font")) {
                QFont f = d.splitView->currentView()->textBrowser()->font();
                if (value.isEmpty())
                    f.setFamily(font().family());
                else
                    f.setFamily(value);
                d.splitView->currentView()->textBrowser()->setFont(f);
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
    bufferModel->setMonitorEnabled(true);
    // give bouncers 2 seconds to start joining channels, otherwise a
    // non-bouncer connection is assumed and model state is restored
    bufferModel->setJoinDelay(2);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    serverBuffer->setSticky(true);
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));

    connect(bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(addBuffer(IrcBuffer*)));
    connect(connection, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));
    connect(connection, SIGNAL(secureError()), this, SLOT(onSecureError()));
    connect(connection, SIGNAL(connected()), this, SLOT(onConnected()));

    MessageHandler* handler = new MessageHandler(bufferModel);
    handler->setDefaultBuffer(serverBuffer);
    handler->setCurrentBuffer(serverBuffer);

    addBuffer(serverBuffer);
    if (!d.treeWidget->currentBuffer())
        d.treeWidget->setCurrentBuffer(serverBuffer);

    connection->installCommandFilter(this);
    if (!connection->isActive() && connection->isEnabled() && !QSettings().value("offline", false).toBool())
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

    QString id = buffer->connection()->userData().value("uuid").toString();
    id += "/" + buffer->title();
    doc->setTimestamp(d.timestamps.value(id).toDateTime());

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

    connect(document, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
    connect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onAlert(IrcMessage*)));
    connect(document, SIGNAL(privateMessageReceived(IrcMessage*)), this, SLOT(onAlert(IrcMessage*)));
}

void ChatPage::addView(BufferView* view)
{
    TitleBar* bar = view->titleBar();
    bar->setStyleSheet(d.theme.style());

#if QT_VERSION >= 0x050300 && !defined(Q_OS_MAC)
    view->textBrowser()->verticalScrollBar()->setStyle(ScrollBarStyle::expanding());
    view->listView()->verticalScrollBar()->setStyle(ScrollBarStyle::expanding());
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

void ChatPage::onCurrentBufferChanged(IrcBuffer* buffer)
{
    MessageHandler* handler = buffer->model()->findChild<MessageHandler*>();
    if (handler)
        handler->setCurrentBuffer(buffer);
}

void ChatPage::onCurrentViewChanged(BufferView* current, BufferView* previous)
{
    d.finder->cancelListSearch(previous);
    d.finder->cancelBrowserSearch(previous);
    emit currentViewChanged(current);
}

void ChatPage::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        TextDocument* doc = qobject_cast<TextDocument*>(sender());
        if (doc && !doc->isClone()) {
            IrcBuffer* buffer = doc->buffer();
            TreeItem* item = d.treeWidget->bufferItem(buffer);
            if (buffer && item != d.treeWidget->currentItem()) {
                bool visible = false;
                foreach (TextDocument* doc, buffer->findChildren<TextDocument*>()) {
                    if (doc->isVisible()) {
                        visible = true;
                        break;
                    }
                }
                // exclude broadcasted global notices
                if (!visible && (message->type() != IrcMessage::Notice || static_cast<IrcNoticeMessage*>(message)->target() != "$$*"))
                    item->setData(1, TreeRole::Badge, item->data(1, TreeRole::Badge).toInt() + 1);
            }
        }
    }
}

void ChatPage::onAlert(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        emit alert(message);
        TextDocument* doc = qobject_cast<TextDocument*>(sender());
        if (doc && !doc->isVisible()) {
            IrcBuffer* buffer = doc->buffer();
            TreeItem* item = d.treeWidget->bufferItem(buffer);
            if (buffer && item != d.treeWidget->currentItem())
                d.treeWidget->highlightItem(item);
        }
    }
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

void ChatPage::onSecureError()
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection && connection->status() == IrcConnection::Error) {
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>(); // TODO
        if (model) {
            IrcBuffer* buffer = model->get(0);
            if (buffer) {
                QStringList params = QStringList() << connection->nickName() << tr("Unable to establish secure connection.");
                IrcMessage* message = IrcMessage::fromParameters(buffer->title(), QString::number(Irc::ERR_UNKNOWNERROR), params, connection);
                foreach (TextDocument* doc, buffer->findChildren<TextDocument*>())
                    doc->receiveMessage(message);
                delete message;
            }
        }
    }
}

void ChatPage::onConnected()
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection) {
        TreeItem* item = d.treeWidget->connectionItem(connection);
        if (item)
            d.treeWidget->unhighlightItem(item);
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
    parser->addCommand(IrcCommand::Who, "WHO <mask>");
    parser->addCommand(IrcCommand::Whois, "WHOIS <user>");
    parser->addCommand(IrcCommand::Whowas, "WHOWAS <user>");

    parser->addCommand(IrcCommand::Custom, "CLEAR");
    parser->addCommand(IrcCommand::Custom, "CLOSE");
    parser->addCommand(IrcCommand::Custom, "MSG <user/channel> <message...>");
    parser->addCommand(IrcCommand::Custom, "QUERY <user> (<message...>)");
    parser->addCommand(IrcCommand::Custom, "SET <key> (<value...>)");

    return parser;
}
