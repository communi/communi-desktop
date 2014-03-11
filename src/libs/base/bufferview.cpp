/*
* Copyright (C) 2008-2014 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "bufferview.h"
#include "application.h"
#include "settingsmodel.h"
#include "syntaxhighlighter.h"
#include "messagestackview.h"
#include "messageformatter.h"
#include "ignoremanager.h"
#include "commandparser.h"
#include "connection.h"
#include <QAbstractTextDocumentLayout>
#include <QDesktopServices>
#include <QScrollBar>
#include <QTextBlock>
#include <QShortcut>
#include <QKeyEvent>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QUrl>
#include <irctextformat.h>
#include <irccompleter.h>
#include <ircusermodel.h>
#include <ircpalette.h>
#include <ircmessage.h>
#include <irccommand.h>
#include <ircchannel.h>
#include <ircbuffer.h>
#include <irc.h>

BufferView::BufferView(ViewInfo::Type type, IrcConnection* connection, MessageStackView* stackView) :
    QWidget(stackView)
{
    d.setupUi(this);
    d.viewType = type;
    d.sentId = 1;
#if QT_VERSION >= 0x040700
    d.awayReply.invalidate();
#else
    d.awayReply = QTime();
#endif
    d.parser = stackView->parser();
    d.firstNames = true;

    d.connected = 0;
    d.disconnected = 0;

    d.topicLabel->setMinimumHeight(d.textInput->sizeHint().height());
    d.helpLabel->setMinimumHeight(d.textInput->sizeHint().height());

    connect(d.splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved()));

    setFocusProxy(d.textInput);
    d.textBrowser->setBuddy(d.textInput);
    d.textBrowser->viewport()->installEventFilter(this);
    connect(d.textBrowser, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));

    d.highlighter = new SyntaxHighlighter(d.textBrowser->document());

    d.connection = connection;
    connect(d.connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(onConnectionStatusChanged()));

    if (type == ViewInfo::Server)
        connect(d.connection, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));

    d.topicLabel->setVisible(type == ViewInfo::Channel);
    d.listView->setVisible(type == ViewInfo::Channel);
    if (type == ViewInfo::Channel) {
        d.listView->setConnection(connection);
        connect(d.listView, SIGNAL(queried(QString)), this, SIGNAL(queried(QString)));
        connect(d.listView, SIGNAL(doubleClicked(QString)), this, SIGNAL(queried(QString)));
        connect(d.listView, SIGNAL(commandRequested(IrcCommand*)), d.connection, SLOT(sendCommand(IrcCommand*)));
        connect(d.topicLabel, SIGNAL(edited(QString)), this, SLOT(onTopicEdited(QString)));
    } else if (type == ViewInfo::Server) {
        connect(d.connection, SIGNAL(connected()), this, SLOT(onConnected()));
        connect(d.connection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    }

    d.formatter = new MessageFormatter(this);
    IrcTextFormat* format = d.formatter->textFormat();
    format->setSpanFormat(IrcTextFormat::SpanStyle);
    IrcPalette* palette = format->palette();
    palette->setColorName(Irc::Gray, "#606060");
    palette->setColorName(Irc::LightGray, "#808080");

    // http://ethanschoonover.com/solarized
    palette->setColorName(Irc::Blue, "#268bd2");
    palette->setColorName(Irc::Green, "#859900");
    palette->setColorName(Irc::Red, "#dc322f");
    palette->setColorName(Irc::Brown, "#cb4b16");
    palette->setColorName(Irc::Purple, "#6c71c4");
    palette->setColorName(Irc::Orange, "#cb4b16");
    palette->setColorName(Irc::Yellow, "#b58900");
    palette->setColorName(Irc::LightGreen, "#859900");
    palette->setColorName(Irc::Cyan, "#2aa198");
    palette->setColorName(Irc::LightCyan, "#2aa198");
    palette->setColorName(Irc::LightBlue, "#268bd2");
    palette->setColorName(Irc::Pink, "#6c71c4");

    d.textBrowser->document()->setDefaultStyleSheet(
        QString(
            ".highlight { color: #ff4040 }"
            ".notice    { color: #a54242 }"
            ".action    { color: #8b388b }"
            ".event     { color: #808080 }"
            ".timestamp { color: #808080; font-size: small }"
            "a { color: #4040ff }"));

    QTextDocument* doc = d.topicLabel->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet("a { color: #4040ff }");

    d.highlighter->setHighlightColor(QColor("#808080"));

    d.textInput->completer()->setParser(d.parser);

    connect(d.textInput, SIGNAL(send(QString)), this, SLOT(sendMessage(QString)));
    connect(d.textInput, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

    connect(d.textInput, SIGNAL(scrollToTop()), d.textBrowser, SLOT(scrollToTop()));
    connect(d.textInput, SIGNAL(scrollToBottom()), d.textBrowser, SLOT(scrollToBottom()));
    connect(d.textInput, SIGNAL(scrollToNextPage()), d.textBrowser, SLOT(scrollToNextPage()));
    connect(d.textInput, SIGNAL(scrollToPreviousPage()), d.textBrowser, SLOT(scrollToPreviousPage()));

    d.helpLabel->hide();
    d.searchEditor->setTextEdit(d.textBrowser);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    applySettings();
    connect(Application::settings(), SIGNAL(changed()), this, SLOT(applySettings()));
}

BufferView::~BufferView()
{
    if (d.buffer) {
        if (d.buffer->isChannel() && d.buffer->isActive())
            connect(d.buffer, SIGNAL(activeChanged(bool)), d.buffer, SLOT(deleteLater()));
        else
            d.buffer->deleteLater();
    }
}

bool BufferView::isActive() const
{
    if (d.buffer)
        return d.buffer->isActive();
    return d.connection && d.connection->isConnected();
}

ViewInfo::Type BufferView::viewType() const
{
    return d.viewType;
}

IrcConnection* BufferView::connection() const
{
    return d.connection;
}

IrcCompleter* BufferView::completer() const
{
    return d.textInput->completer();
}

QTextBrowser* BufferView::textBrowser() const
{
    return d.textBrowser;
}

QString BufferView::receiver() const
{
    return d.receiver;
}

void BufferView::setReceiver(const QString& receiver)
{
    if (d.receiver != receiver) {
        d.receiver = receiver;
        emit receiverChanged(receiver);
    }
}

IrcBuffer* BufferView::buffer() const
{
    return d.buffer;
}

void BufferView::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (IrcChannel* channel = qobject_cast<IrcChannel*>(buffer))
            d.listView->setChannel(channel);
        d.buffer = buffer;
        d.formatter->setBuffer(buffer);
        d.textInput->completer()->setBuffer(buffer);
        connect(buffer, SIGNAL(activeChanged(bool)), this, SIGNAL(activeChanged()));
        connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));
        connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
    }
}

QByteArray BufferView::saveSplitter() const
{
    if (d.viewType != ViewInfo::Server)
        return d.splitter->saveState();
    return QByteArray();
}

void BufferView::restoreSplitter(const QByteArray& state)
{
    d.splitter->restoreState(state);
}

void BufferView::showHelp(const QString& text, bool error)
{
    QString syntax;
    if (text == "/") {
        QStringList commands = d.parser->availableCommands();
        syntax = commands.join(" ");
    } else if (text.startsWith('/')) {
        QStringList words = text.mid(1).split(' ');
        QString command = words.value(0);
        QStringList suggestions = d.parser->suggestedCommands(command, words.mid(1));
        if (suggestions.count() == 1)
            syntax = d.parser->visualSyntax(suggestions.first());
        else
            syntax = suggestions.join(" ");

        if (syntax.isEmpty())
            syntax = tr("SERVER COMMAND: %1 %2").arg(command.toUpper(), QStringList(words.mid(1)).join(" "));
    }

    d.helpLabel->setVisible(!syntax.isEmpty());
    QPalette pal;
    if (error)
        pal.setColor(QPalette::WindowText, "#ff4040");
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);
}

void BufferView::sendMessage(const QString& message)
{
    d.parser->setTarget(d.receiver);
    IrcCommand* cmd = d.parser->parseCommand(message);
    if (cmd) {
        if (cmd->type() == IrcCommand::Custom) {
            QString command = cmd->parameters().value(0);
            QStringList params = cmd->parameters().mid(1);
            if (command == "CLEAR") {
                d.textBrowser->clear();
            } else if (command == "CLOSE") {
                QMetaObject::invokeMethod(window(), "closeView");
            } else if (command == "MSG") {
                // support "/msg foo /cmd" without executing /cmd
                QString msg = QStringList(params.mid(1)).join(" ");
                if (msg.startsWith('/') && !msg.startsWith("//") && !msg.startsWith("/ "))
                    msg.prepend('/');
                emit messaged(params.value(0), msg);
            } else if (command == "QUERY") {
                emit queried(params.value(0));
            } else if (command == "IGNORE") {
                QString ignore  = params.value(0);
                if (ignore.isEmpty()) {
                    const QStringList ignores = IgnoreManager::instance()->ignores();
                    if (!ignores.isEmpty()) {
                        foreach (const QString& ignore, ignores)
                            d.textBrowser->append(d.formatter->formatLine("! " + ignore));
                    } else {
                        d.textBrowser->append(d.formatter->formatLine("! no ignores"));
                    }
                } else {
                    QString mask = IgnoreManager::instance()->addIgnore(ignore);
                    d.textBrowser->append(d.formatter->formatLine("! ignored: " + mask));
                }
            } else if (command == "UNIGNORE") {
                QString mask = IgnoreManager::instance()->removeIgnore(params.value(0));
                d.textBrowser->append(d.formatter->formatLine("! unignored: " + mask));
            }
            delete cmd;
        } else if (cmd->type() == IrcCommand::Message || cmd->type() == IrcCommand::CtcpAction || cmd->type() == IrcCommand::Notice) {
            if (Connection* s = qobject_cast<Connection*>(d.connection)) // TODO
                s->sendUiCommand(cmd, QString("_communi_msg_%1_%2").arg(d.receiver).arg(++d.sentId));

            IrcMessage* msg = IrcMessage::fromData(":" + d.connection->nickName().toUtf8() + " " + cmd->toString().toUtf8(), d.connection);
            receiveMessage(msg);
            delete msg;

            // highlight as gray until acked
            QTextBlock block = d.textBrowser->document()->lastBlock();
            block.setUserState(d.sentId);
            d.highlighter->rehighlightBlock(block);
        } else {
            if (Connection* s = qobject_cast<Connection*>(d.connection)) // TODO
                s->sendUiCommand(cmd);
        }
        d.helpLabel->hide();
    } else {
        showHelp(message, true);
    }
}

void BufferView::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    onEscPressed();
}

bool BufferView::eventFilter(QObject* object, QEvent* event)
{
    if (object == d.textBrowser->viewport() && event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* menuEvent = static_cast<QContextMenuEvent*>(event);

        QPoint pt = menuEvent->pos();
        pt.rx() += d.textBrowser->isRightToLeft() ? d.textBrowser->horizontalScrollBar()->maximum() - d.textBrowser->horizontalScrollBar()->value() : d.textBrowser->horizontalScrollBar()->value();
        pt.ry() += d.textBrowser->verticalScrollBar()->value();
        QMenu* menu = d.textBrowser->createStandardContextMenu(pt);

        QAction* query = 0;
        QAction* whois = 0;
        QAction* users = 0;
        QAction* views = 0;

        QUrl link(d.textBrowser->anchorAt(menuEvent->pos()));
        if (link.scheme() == "nick") {
            QAction* separator = menu->insertSeparator(menu->actions().value(0));

            QTextCursor cursor = d.textBrowser->cursorForPosition(menuEvent->pos());
            cursor.select(QTextCursor::WordUnderCursor);
            d.textBrowser->setTextCursor(cursor);

            query = new QAction(tr("Query"), menu);
            menu->insertAction(separator, query);

            whois = new QAction(tr("Whois"), menu);
            menu->insertAction(query, whois);
        }

        QAction* separator = 0;
        if (d.viewType == ViewInfo::Channel && d.splitter->sizes().value(1) == 0) {
            separator = menu->addSeparator();
            users = menu->addAction(tr("Restore users"));
        }

        QSplitter* splitter = window()->findChild<QSplitter*>();
        if (splitter && splitter->sizes().value(0) == 0) {
            if (!separator)
                menu->addSeparator();
            views = menu->addAction(tr("Restore views"));
        }

        QAction* action = menu->exec(menuEvent->globalPos());
        if (action) {
            const QString user = link.toString(QUrl::RemoveScheme);
            if (action == whois) {
                IrcCommand* command = IrcCommand::createWhois(user);
                d.connection->sendCommand(command);
            } else if (action == query) {
                emit queried(user);
            } else if (action == users) {
                d.splitter->setSizes(QList<int>() << d.textBrowser->sizeHint().width() << d.listView->sizeHint().width());
                onSplitterMoved();
            } else if (action == views) {
                splitter->setSizes(QList<int>() << splitter->widget(0)->sizeHint().width() << splitter->widget(1)->sizeHint().width());
            }
        }

        menu->deleteLater();
        return true;
    }
    return QWidget::eventFilter(object, event);
}

void BufferView::onConnected()
{
    ++d.connected;

    int blocks = d.textBrowser->document()->blockCount();
    if (blocks > 10) // TODO
        d.textBrowser->addMarker(blocks);
}

void BufferView::onDisconnected()
{
    ++d.disconnected;
}

void BufferView::onEscPressed()
{
    d.helpLabel->hide();
    d.searchEditor->hide();
    setFocus(Qt::OtherFocusReason);
}

void BufferView::onTitleChanged(const QString& title)
{
    const QString old = d.receiver;
    setReceiver(title);
    emit renamed(old, title);
}

void BufferView::onSplitterMoved()
{
    emit splitterChanged(saveSplitter());
}

void BufferView::onAnchorClicked(const QUrl& link)
{
    if (link.scheme() == "nick") {
        emit queried(link.toString(QUrl::RemoveScheme));
    } else {
        QDesktopServices::openUrl(link);
        // avoid focus rectangle around the link
        d.textInput->setFocus();
    }
}

void BufferView::onTopicEdited(const QString& topic)
{
    d.connection->sendCommand(IrcCommand::createTopic(d.receiver, topic));
}

void BufferView::onConnectionStatusChanged()
{
    d.textInput->setFocusPolicy(d.connection->isActive() ? Qt::StrongFocus : Qt::NoFocus);
    d.textBrowser->setFocusPolicy(d.connection->isActive() ? Qt::StrongFocus : Qt::NoFocus);
}

void BufferView::onSocketError()
{
    QString msg = tr("[ERROR] %1").arg(d.connection->socket()->errorString());
    d.textBrowser->append(d.formatter->formatLine(msg));
}

void BufferView::applySettings()
{
    SettingsModel* settings = Application::settings();
    bool dark =  settings->value("ui.dark").toBool();

    d.textBrowser->setFont(settings->value("ui.font").value<QFont>());
    d.textBrowser->document()->setMaximumBlockCount(settings->value("ui.scrollback").toInt());

    if (dark) {
        d.textBrowser->setShadowColor(Qt::black);
        d.textBrowser->setMarkerColor(QColor("#ffffff"));
        d.textBrowser->setHighlightColor(QColor("#ff4040").darker(465));

        d.searchEditor->setButtonPixmap(SearchEditor::Left, QPixmap(":/resources/buttons/prev-white.png"));
        d.searchEditor->setButtonPixmap(SearchEditor::Right, QPixmap(":/resources/buttons/next-white.png"));

        d.textInput->setButtonPixmap(TextInput::Right, QPixmap(":/resources/buttons/return-white.png"));
        d.textInput->setButtonPixmap(TextInput::Left, QPixmap(":/resources/buttons/tab-white.png"));
    } else {
        d.textBrowser->setShadowColor(Qt::gray);
        d.textBrowser->setMarkerColor(QColor("#000000"));
        d.textBrowser->setHighlightColor(QColor("#ff4040").lighter(165));

        d.searchEditor->setButtonPixmap(SearchEditor::Left, QPixmap(":/resources/prev-black.png"));
        d.searchEditor->setButtonPixmap(SearchEditor::Right, QPixmap(":/resources/next-black.png"));

        d.textInput->setButtonPixmap(TextInput::Right, QPixmap(":/resources/buttons/return-black.png"));
        d.textInput->setButtonPixmap(TextInput::Left, QPixmap(":/resources/buttons/tab-black.png"));
    }

    d.showEvents = settings->value("messages.events").toBool();
    d.formatter->setDetailed(settings->value("formatting.detailed").toBool());
    d.formatter->setStripNicks(settings->value("formatting.hideUserHosts").toBool());
    d.formatter->setTimeStampFormat(settings->value("formatting.timeStamp").toString());
}

void BufferView::receiveMessage(IrcMessage* message)
{
    bool ignore = false;
    bool highlight = false;

    switch (message->type()) {
        case IrcMessage::Private: {
            if (message->nick() == QLatin1String("***") && message->ident() == QLatin1String("znc")) {
                QString content = static_cast<IrcPrivateMessage*>(message)->content();
                if (content == QLatin1String("Buffer Playback..."))
                    ignore = true;
                else if (content == QLatin1String("Playback Complete."))
                    ignore = true;
            }
            if (!(message->flags() & IrcMessage::Own) && static_cast<IrcPrivateMessage*>(message)->content().contains(d.connection->nickName()))
                highlight = true;
            break;
        }
        case IrcMessage::Notice:
            if (!(message->flags() & IrcMessage::Own) && static_cast<IrcNoticeMessage*>(message)->content().contains(d.connection->nickName()))
                highlight = true;
            break;
        case IrcMessage::Topic:
            d.topic = static_cast<IrcTopicMessage*>(message)->topic();
            d.topicLabel->setText(d.formatter->formatContent(d.topic));
            if (d.topicLabel->text().isEmpty())
                d.topicLabel->setText(tr("-"));
            break;
        case IrcMessage::Join:
            if (message->flags() & IrcMessage::Own) {
                d.firstNames = true;
                int blocks = d.textBrowser->document()->blockCount();
                if (blocks > 1)
                    d.textBrowser->addMarker(blocks);
            }
            if (!d.showEvents)
                return;
            break;
        case IrcMessage::Quit:
            if (!d.showEvents)
                return;
            break;
        case IrcMessage::Part:
            if (!d.showEvents)
                return;
            break;
        case IrcMessage::Pong: {
            QString arg = static_cast<IrcPongMessage*>(message)->argument();
            if (arg.startsWith("_communi_msg_")) {
                int id = arg.mid(arg.lastIndexOf("_") + 1).toInt();
                if (id > 0) {
                    QTextBlock block = d.textBrowser->document()->lastBlock();
                    while (block.isValid()) {
                        if (block.userState() == id) {
                            block.setUserState(-1);
                            d.highlighter->rehighlightBlock(block);
                            break;
                        }
                        block = block.previous();
                    }
                }
            }
            break;
        }
        case IrcMessage::Numeric:
            switch (static_cast<IrcNumericMessage*>(message)->code()) {
                case Irc::RPL_TOPICWHOTIME: {
                    QDateTime dateTime = QDateTime::fromTime_t(message->parameters().value(3).toInt());
                    d.topicLabel->setToolTip(tr("Set %1 by %2").arg(dateTime.toString(), message->parameters().value(2)));
                    break;
                }
                case Irc::RPL_AWAY: // TODO: configurable
                    if (d.awayReply.isValid() && d.awayReply.elapsed() < 1800000
                            && message->parameters().last() == d.awayMessage) {
                        return;
                    }
                    d.awayReply.restart();
                    d.awayMessage = message->parameters().last();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if (d.viewType == ViewInfo::Channel) {
        if (message->type() == IrcMessage::Names) {
            // - short version on first join
            // - ignore names on consecutive joins
            // - long version as reply to /names
            bool wasFirst = d.firstNames;
            d.firstNames = false;
            if (wasFirst)
                return;
        }
    }

    QString formatted = d.formatter->formatMessage(message);
    if (formatted.length()) {
        if (!ignore && (!isVisible() || !isActiveWindow())) {
            IrcMessage::Type type = message->type();
            if (highlight || ((type == IrcMessage::Notice || type == IrcMessage::Private) && d.viewType != ViewInfo::Channel))
                emit highlighted(message);
            else if (type == IrcMessage::Notice || type == IrcMessage::Private) // TODO: || (!d.receivedCodes.contains(Irc::RPL_ENDOFMOTD) && d.viewType == ViewInfo::Server))
                emit missed(message);
        }
        d.textBrowser->append(formatted, highlight);
    }
}

bool BufferView::hasUser(const QString& user) const
{
    return (!d.connection->nickName().compare(user, Qt::CaseInsensitive) && d.viewType != ViewInfo::Query) ||
           (d.viewType == ViewInfo::Query && !d.receiver.compare(user, Qt::CaseInsensitive)) ||
           (d.viewType == ViewInfo::Channel && d.listView->hasUser(user));
}

void BufferView::updateLag(qint64 lag)
{
    d.textInput->setLag(lag);
}
