/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "messageview.h"
#include "syntaxhighlighter.h"
#include "commandparser.h"
#include "menufactory.h"
#include "completer.h"
#include "usermodel.h"
#include "session.h"
#include <QAbstractTextDocumentLayout>
#include <QDesktopServices>
#include <QStringListModel>
#include <QTextBlock>
#include <QShortcut>
#include <QKeyEvent>
#include <QDateTime>
#include <QDebug>
#include <QUrl>
#include <ircmessage.h>
#include <irccommand.h>
#include <irc.h>

static QStringListModel* command_model = 0;

MessageView::MessageView(ViewInfo::Type type, Session* session, QWidget* parent) :
    QWidget(parent)
{
    d.setupUi(this);
    d.viewType = type;
    d.joined = false;
    d.sentId = 1;
    d.awayReply.invalidate();

    d.topicLabel->setMinimumHeight(d.lineEditor->sizeHint().height());
    d.helpLabel->setMinimumHeight(d.lineEditor->sizeHint().height());

    connect(d.splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved()));

    setFocusProxy(d.lineEditor);
    d.textBrowser->setBuddy(d.lineEditor);
    d.textBrowser->viewport()->installEventFilter(this);
    connect(d.textBrowser, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));

    d.formatter = new MessageFormatter(this);
    d.highlighter = new SyntaxHighlighter(d.textBrowser->document());

    d.session = session;
    connect(d.session, SIGNAL(activeChanged(bool)), this, SIGNAL(activeChanged()));
    connect(d.session, SIGNAL(connectedChanged(bool)), this, SIGNAL(activeChanged()));
    connect(d.session->lagMeter(), SIGNAL(lagChanged(int)), d.lineEditor, SLOT(setLag(int)));
    d.lineEditor->setLag(d.session->lagMeter()->lag());
    if (type == ViewInfo::Server)
        connect(d.session, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(onSocketError()));

    d.topicLabel->setVisible(type == ViewInfo::Channel);
    d.listView->setVisible(type == ViewInfo::Channel);
    if (type == ViewInfo::Channel) {
        d.listView->setSession(session);
        connect(d.listView, SIGNAL(queried(QString)), this, SIGNAL(queried(QString)));
        connect(d.listView, SIGNAL(doubleClicked(QString)), this, SIGNAL(queried(QString)));
        connect(d.listView, SIGNAL(commandRequested(IrcCommand*)), d.session, SLOT(sendCommand(IrcCommand*)));
        connect(d.topicLabel, SIGNAL(edited(QString)), this, SLOT(onTopicEdited(QString)));
    }

    if (!command_model) {
        command_model = new QStringListModel(qApp);

        CommandParser::addCustomCommand("CLEAR", "");
        CommandParser::addCustomCommand("QUERY", "<user>");
        CommandParser::addCustomCommand("MSG", "<usr/channel> <message...>");
    }

    d.lineEditor->completer()->setUserModel(d.listView->userModel());
    d.lineEditor->completer()->setCommandModel(command_model);
    connect(d.lineEditor->completer(), SIGNAL(commandCompletion(QString)), this, SLOT(completeCommand(QString)));

    connect(d.lineEditor, SIGNAL(send(QString)), this, SLOT(sendMessage(QString)));
    connect(d.lineEditor, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

    connect(d.lineEditor, SIGNAL(scrollToTop()), d.textBrowser, SLOT(scrollToTop()));
    connect(d.lineEditor, SIGNAL(scrollToBottom()), d.textBrowser, SLOT(scrollToBottom()));
    connect(d.lineEditor, SIGNAL(scrollToNextPage()), d.textBrowser, SLOT(scrollToNextPage()));
    connect(d.lineEditor, SIGNAL(scrollToPreviousPage()), d.textBrowser, SLOT(scrollToPreviousPage()));

    d.helpLabel->hide();
    d.searchEditor->setTextEdit(d.textBrowser);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));
}

MessageView::~MessageView()
{
}

bool MessageView::isActive() const
{
    switch (d.viewType) {
    case ViewInfo::Channel: return d.joined;
    case ViewInfo::Server: return d.session && d.session->isActive();
    case ViewInfo::Query: return d.session && d.session->isConnected();
    default: return false;
    }
}

ViewInfo::Type MessageView::viewType() const
{
    return d.viewType;
}

Session* MessageView::session() const
{
    return d.session;
}

UserModel* MessageView::userModel() const
{
    return d.listView->userModel();
}

Completer* MessageView::completer() const
{
    return d.lineEditor->completer();
}

QTextBrowser* MessageView::textBrowser() const
{
    return d.textBrowser;
}

MessageFormatter* MessageView::messageFormatter() const
{
    return d.formatter;
}

QString MessageView::receiver() const
{
    return d.receiver;
}

void MessageView::setReceiver(const QString& receiver)
{
    if (d.receiver != receiver) {
        d.receiver = receiver;
        if (d.viewType == ViewInfo::Channel)
            d.listView->setChannel(receiver);
        emit receiverChanged(receiver);
    }
}

MenuFactory* MessageView::menuFactory() const
{
    return d.listView->menuFactory();
}

void MessageView::setMenuFactory(MenuFactory* factory)
{
    d.listView->setMenuFactory(factory);
}

QByteArray MessageView::saveSplitter() const
{
    if (d.viewType != ViewInfo::Server)
        return d.splitter->saveState();
    return QByteArray();
}

void MessageView::restoreSplitter(const QByteArray& state)
{
    d.splitter->restoreState(state);
}

void MessageView::showHelp(const QString& text, bool error)
{
    QString syntax;
    if (text == "/") {
        QStringList commands = CommandParser::availableCommands();
        syntax = commands.join(" ");
    } else if (text.startsWith('/')) {
        QStringList words = text.mid(1).split(' ');
        QString command = words.value(0);
        QStringList suggestions = CommandParser::suggestedCommands(command, words.mid(1));
        if (suggestions.count() == 1)
            syntax = CommandParser::syntax(suggestions.first());
        else
            syntax = suggestions.join(" ");

        if (syntax.isEmpty() && error)
            syntax = tr("Unknown command '%1'").arg(command.toUpper());
    }

    d.helpLabel->setVisible(!syntax.isEmpty());
    QPalette pal;
    if (error)
        pal.setColor(QPalette::WindowText, d.settings.colors[Settings::Highlight]);
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);
}

void MessageView::sendMessage(const QString& message)
{
    QStringList lines = message.split(QRegExp("[\\r\\n]"), QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        IrcCommand* cmd = CommandParser::parseCommand(d.receiver, line);
        if (cmd) {
            if (cmd->type() == IrcCommand::Custom) {
                QString command = cmd->parameters().value(0);
                QStringList params = cmd->parameters().mid(1);
                if (command == "CLEAR") {
                    d.textBrowser->clear();
                } else if (command == "MSG") {
                    // support "/msg foo /cmd" without executing /cmd
                    QString msg = QStringList(params.mid(1)).join(" ");
                    if (msg.startsWith('/') && !msg.startsWith("//") && !msg.startsWith("/ "))
                        msg.prepend('/');
                    emit messaged(params.value(0), msg);
                } else if (command == "QUERY") {
                    emit queried(params.value(0));
                }
                delete cmd;
            } else {
                d.session->sendCommand(cmd);

                if (cmd->type() == IrcCommand::Message || cmd->type() == IrcCommand::CtcpAction || cmd->type() == IrcCommand::Notice) {
                    IrcMessage* msg = IrcMessage::fromData(":" + d.session->nickName().toUtf8() + " " + cmd->toString().toUtf8(), d.session);
                    receiveMessage(msg);
                    delete msg;

                    // highlight as gray until acked
                    d.session->sendData("PING _communi_msg_" + d.receiver.toUtf8() + "_" + QByteArray::number(d.sentId));
                    QTextBlock block = d.textBrowser->document()->lastBlock();
                    block.setUserState(d.sentId);
                    d.highlighter->rehighlightBlock(block);
                    d.sentId++;
                }
            }
        } else {
            showHelp(line, true);
        }
    }
}

void MessageView::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    onEscPressed();
}

bool MessageView::eventFilter(QObject* object, QEvent* event)
{
    if (object == d.textBrowser->viewport() && event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* menuEvent = static_cast<QContextMenuEvent*>(event);
        QAbstractTextDocumentLayout* layout = d.textBrowser->document()->documentLayout();
        QUrl link(layout->anchorAt(menuEvent->pos()));
        if (link.scheme() == "nick") {
            QMenu* standardMenu = d.textBrowser->createStandardContextMenu(menuEvent->pos());
            QMenu* customMenu = d.listView->menuFactory()->createUserViewMenu(link.toString(QUrl::RemoveScheme), this);
            customMenu->addSeparator();
            customMenu->insertActions(0, standardMenu->actions());
            customMenu->exec(menuEvent->globalPos());
            customMenu->deleteLater();
            delete standardMenu;
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

void MessageView::onEscPressed()
{
    d.helpLabel->hide();
    d.searchEditor->hide();
    setFocus(Qt::OtherFocusReason);
}

void MessageView::onSplitterMoved()
{
    emit splitterChanged(saveSplitter());
}

void MessageView::onAnchorClicked(const QUrl& link)
{
    if (link.scheme() == "nick")
        emit queried(link.toString(QUrl::RemoveScheme));
    else
        QDesktopServices::openUrl(link);
}

void MessageView::completeCommand(const QString& command)
{
    if (command == "TOPIC")
        d.lineEditor->insert(d.topic);
}

void MessageView::onTopicEdited(const QString& topic)
{
    d.session->sendCommand(IrcCommand::createTopic(d.receiver, topic));
}

void MessageView::onSocketError()
{
    QString msg = tr("[ERROR] %1").arg(d.session->socket()->errorString());
    QString formatted = d.formatter->formatMessage(QDateTime::currentDateTime(), msg);
    d.textBrowser->append(formatted);
}

void MessageView::applySettings(const Settings& settings)
{
    CommandParser::setAliases(settings.aliases);

    QStringList commands;
    foreach (const QString& command, CommandParser::availableCommands())
        commands += "/" + command;
    command_model->setStringList(commands);

    d.formatter->setTimeStamp(settings.timeStamp);
    d.formatter->setTimeStampFormat(settings.timeStampFormat);
    d.formatter->setStripNicks(settings.stripNicks);

    // TODO: dedicated colors?
    d.highlighter->setHighlightColor(settings.colors.value(Settings::TimeStamp));
    d.textBrowser->setHighlightColor(QColor(settings.colors.value(Settings::Highlight)).lighter(165));

    if (!settings.font.isEmpty())
        d.textBrowser->setFont(settings.font);
    d.textBrowser->document()->setMaximumBlockCount(settings.maxBlockCount);

    QTextDocument* doc = d.topicLabel->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet(QString("a { color: %1 }").arg(settings.colors.value(Settings::Link)));

    QString backgroundColor = settings.colors.value(Settings::Background);
    d.textBrowser->setStyleSheet(QString("QTextBrowser { background-color: %1 }").arg(backgroundColor));

    d.textBrowser->document()->setDefaultStyleSheet(
        QString(
            ".highlight { color: %1 }"
            ".message   { color: %2 }"
            ".notice    { color: %3 }"
            ".action    { color: %4 }"
            ".event     { color: %5 }"
            ".timestamp { color: %6; font-size: small }"
            "a { color: %7 }"
        ).arg(settings.colors.value(Settings::Highlight))
        .arg(settings.colors.value(Settings::Message))
        .arg(settings.colors.value(Settings::Notice))
        .arg(settings.colors.value(Settings::Action))
        .arg(settings.colors.value(Settings::Event))
        .arg(settings.colors.value(Settings::TimeStamp))
        .arg(settings.colors.value(Settings::Link)));
    d.settings = settings;
}

void MessageView::receiveMessage(IrcMessage* message)
{
    if (d.viewType == ViewInfo::Channel)
        d.listView->processMessage(message);

    bool ignore = false;
    switch (message->type()) {
        case IrcMessage::Private: {
            IrcSender sender = message->sender();
            if (sender.name() == QLatin1String("***") && sender.user() == QLatin1String("znc")) {
                QString content = static_cast<IrcPrivateMessage*>(message)->message();
                if (content == QLatin1String("Buffer Playback...")) {
                    d.formatter->setZncPlaybackMode(true);
                    ignore = true;
                } else if (content == QLatin1String("Playback Complete.")) {
                    d.formatter->setZncPlaybackMode(false);
                    ignore = true;
                }
            }
            break;
        }
        case IrcMessage::Topic:
            d.topic = static_cast<IrcTopicMessage*>(message)->topic();
            d.topicLabel->setText(d.formatter->formatHtml(d.topic));
            if (d.topicLabel->text().isEmpty())
                d.topicLabel->setText(tr("-"));
            break;
        case IrcMessage::Join:
            if (message->flags() & IrcMessage::Own) {
                d.joined = true;
                int blocks = d.textBrowser->document()->blockCount();
                if (blocks > 1)
                    d.textBrowser->addMarker(blocks);
                emit activeChanged();
            }
            break;
        case IrcMessage::Quit:
        case IrcMessage::Part:
            if (message->flags() & IrcMessage::Own) {
                d.joined = false;
                emit activeChanged();
            }
            break;
        case IrcMessage::Kick:
            if (!static_cast<IrcKickMessage*>(message)->user().compare(d.session->nickName(), Qt::CaseInsensitive)) {
                d.joined = false;
                emit activeChanged();
            }
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
                case Irc::RPL_NOTOPIC:
                    d.topic.clear();
                    d.topicLabel->setText(tr("-"));
                    break;
                case Irc::RPL_TOPIC:
                    d.topic = message->parameters().value(2);
                    d.topicLabel->setText(d.formatter->formatHtml(d.topic));
                    break;
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

    d.formatter->setUsers(d.listView->userModel()->users());
    d.formatter->setHighlights(QStringList() << d.session->nickName());
    QString formatted = d.formatter->formatMessage(message);
    if (formatted.length()) {
        if (!ignore && (!isVisible() || !isActiveWindow())) {
            IrcMessage::Type type = d.formatter->effectiveMessageType();
            if (d.formatter->hasHighlight() || ((type == IrcMessage::Notice || type == IrcMessage::Private) && d.viewType != ViewInfo::Channel))
                emit highlighted(message);
            else if (type == IrcMessage::Notice || type == IrcMessage::Private) // TODO: || (!d.receivedCodes.contains(Irc::RPL_ENDOFMOTD) && d.viewType == ViewInfo::Server))
                emit missed(message);
        }

        d.textBrowser->append(formatted, d.formatter->hasHighlight());
    }
}

bool MessageView::hasUser(const QString& user) const
{
    return (!d.session->nickName().compare(user, Qt::CaseInsensitive) && d.viewType != ViewInfo::Query) ||
           (d.viewType == ViewInfo::Query && !d.receiver.compare(user, Qt::CaseInsensitive)) ||
           (d.viewType == ViewInfo::Channel && d.listView->hasUser(user));
}
