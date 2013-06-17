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
#include "application.h"
#include "settingsmodel.h"
#include "syntaxhighlighter.h"
#include "useractivitymodel.h"
#include "messagestackview.h"
#include "messageformatter.h"
#include "commandparser.h"
#include "menufactory.h"
#include "completer.h"
#include "session.h"
#include <QAbstractTextDocumentLayout>
#include <QDesktopServices>
#include <QTextBlock>
#include <QShortcut>
#include <QKeyEvent>
#include <QDateTime>
#include <QDebug>
#include <QUrl>
#include <irctextformat.h>
#include <ircusermodel.h>
#include <irclagtimer.h>
#include <ircpalette.h>
#include <ircmessage.h>
#include <irccommand.h>
#include <ircchannel.h>
#include <ircbuffer.h>
#include <irc.h>

Q_GLOBAL_STATIC(IrcTextFormat, irc_text_format)

MessageView::MessageView(ViewInfo::Type type, IrcSession* session, MessageStackView* stackView) :
    QWidget(stackView)
{
    d.setupUi(this);
    d.viewType = type;
    d.sentId = 1;
    d.awayReply.invalidate();
    d.playback = false;
    d.parser = stackView->parser();
    d.buffer = 0;

    d.joined = 0;
    d.parted = 0;
    d.connected = 0;
    d.disconnected = 0;

    d.topicLabel->setMinimumHeight(d.lineEditor->sizeHint().height());
    d.helpLabel->setMinimumHeight(d.lineEditor->sizeHint().height());

    connect(d.splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved()));

    setFocusProxy(d.lineEditor);
    d.textBrowser->setBuddy(d.lineEditor);
    d.textBrowser->viewport()->installEventFilter(this);
    connect(d.textBrowser, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));

    d.highlighter = new SyntaxHighlighter(d.textBrowser->document());

    d.session = session;
    connect(d.session, SIGNAL(activeChanged(bool)), this, SLOT(onSessionStatusChanged()));
    connect(d.session, SIGNAL(connectedChanged(bool)), this, SLOT(onSessionStatusChanged()));

    d.lagTimer = new IrcLagTimer(d.session);
    connect(d.lagTimer, SIGNAL(lagChanged(int)), d.lineEditor, SLOT(setLag(int)));

    d.lineEditor->setLag(d.lagTimer->lag());
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
    } else if (type == ViewInfo::Server) {
        connect(d.session, SIGNAL(connected()), this, SLOT(onConnected()));
        connect(d.session, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    }

    static bool init = false;
    if (!init) {
        init = true;
        IrcPalette palette;
        QStringList colorNames = QStringList()
                << "navy" << "green" << "red" << "maroon" << "purple" << "olive"
                << "yellow" << "lime" << "teal" << "aqua" << "royalblue" << "fuchsia";
        for (int i = IrcPalette::Blue; i <= IrcPalette::Pink; ++i) {
            QColor color(colorNames.takeFirst());
            color.setHsl(color.hue(), 100, 82);
            palette.setColorName(i, color.name());
        }
        palette.setColorName(IrcPalette::Gray, "#606060");
        palette.setColorName(IrcPalette::LightGray, "#808080");
        irc_text_format()->setPalette(palette);
    }

    d.lineEditor->completer()->setCommandModel(stackView->commandModel());
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

    applySettings();
    connect(Application::settings(), SIGNAL(changed()), this, SLOT(applySettings()));
}

MessageView::~MessageView()
{
}

bool MessageView::isActive() const
{
    switch (d.viewType) {
    case ViewInfo::Channel: return (d.joined > d.parted) && d.session && d.session->isConnected();
    case ViewInfo::Server: return d.session && d.session->isActive();
    case ViewInfo::Query: return d.session && d.session->isConnected();
    default: return false;
    }
}

ViewInfo::Type MessageView::viewType() const
{
    return d.viewType;
}

IrcSession* MessageView::session() const
{
    return d.session;
}

IrcUserModel* MessageView::userModel() const
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

QString MessageView::receiver() const
{
    return d.receiver;
}

void MessageView::setReceiver(const QString& receiver)
{
    if (d.receiver != receiver) {
        d.receiver = receiver;
        emit receiverChanged(receiver);
    }
}

IrcBuffer* MessageView::buffer() const
{
    return d.buffer;
}

void MessageView::setBuffer(IrcBuffer* buffer)
{
    if (IrcChannel* channel = qobject_cast<IrcChannel*>(buffer)) {
        d.listView->setChannel(channel);
        d.lineEditor->completer()->setUserModel(new UserActivityModel(channel));
    }
    d.buffer = buffer;
    buffer->setParent(this);
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));
}

bool MessageView::playbackMode() const
{
    return d.playback;
}

void MessageView::setPlaybackMode(bool enabled)
{
    d.playback = enabled;
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

        if (syntax.isEmpty() && error)
            syntax = tr("Unknown command '%1'").arg(command.toUpper());
    }

    d.helpLabel->setVisible(!syntax.isEmpty());
    QPalette pal;
    if (error) {
        SettingsModel* settings = Application::settings();
        QString theme =  settings->value("ui.theme").toString();
        QString key = QString("themes.%1.highlight").arg(theme);
        pal.setColor(QPalette::WindowText, settings->value(key).value<QColor>());
    }
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);
}

void MessageView::sendMessage(const QString& message)
{
    IrcCommand* cmd = d.parser->parseCommand(message);
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
        } else if (cmd->type() == IrcCommand::Message || cmd->type() == IrcCommand::CtcpAction || cmd->type() == IrcCommand::Notice) {
            if (Session* s = qobject_cast<Session*>(d.session)) // TODO
                s->sendUiCommand(cmd, QString("_communi_msg_%1_%2").arg(d.receiver).arg(++d.sentId));

            IrcMessage* msg = IrcMessage::fromData(":" + d.session->nickName().toUtf8() + " " + cmd->toString().toUtf8(), d.session);
            receiveMessage(msg);
            delete msg;

            // highlight as gray until acked
            QTextBlock block = d.textBrowser->document()->lastBlock();
            block.setUserState(d.sentId);
            d.highlighter->rehighlightBlock(block);
        } else {
            d.session->sendCommand(cmd);
        }
    } else {
        showHelp(message, true);
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

void MessageView::onConnected()
{
    ++d.connected;

    int blocks = d.textBrowser->document()->blockCount();
    if (blocks > 10) // TODO
        d.textBrowser->addMarker(blocks);
}

void MessageView::onDisconnected()
{
    ++d.disconnected;
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

void MessageView::onSessionStatusChanged()
{
    d.lineEditor->setFocusPolicy(d.session->isActive() ? Qt::StrongFocus : Qt::NoFocus);
    d.textBrowser->setFocusPolicy(d.session->isActive() ? Qt::StrongFocus : Qt::NoFocus);
    emit activeChanged();
}

void MessageView::onSocketError()
{
    QString msg = tr("[ERROR] %1").arg(d.session->socket()->errorString());
    d.textBrowser->append(MessageFormatter::formatLine(msg));
}

void MessageView::applySettings()
{
    SettingsModel* settings = Application::settings();
    QString theme =  settings->value("ui.theme").toString();

    d.lagTimer->setInterval(settings->value("session.lagTimerInterval").toInt());

    // TODO: dedicated colors?
    d.highlighter->setHighlightColor(settings->value(QString("themes.%1.timestamp").arg(theme)).value<QColor>());
    d.textBrowser->setHighlightColor(settings->value(QString("themes.%1.highlight").arg(theme)).value<QColor>().lighter(165));

    d.textBrowser->setFont(settings->value("ui.font").value<QFont>());
    d.textBrowser->document()->setMaximumBlockCount(settings->value("ui.scrollback").toInt());

    QTextDocument* doc = d.topicLabel->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet(QString("a { color: %1 }").arg(settings->value(QString("themes.%1.link").arg(theme)).toString()));

    QString backgroundColor = settings->value(QString("themes.%1.background").arg(theme)).toString();
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
        ).arg(settings->value(QString("themes.%1.highlight").arg(theme)).toString())
         .arg(settings->value(QString("themes.%1.message").arg(theme)).toString())
         .arg(settings->value(QString("themes.%1.notice").arg(theme)).toString())
         .arg(settings->value(QString("themes.%1.action").arg(theme)).toString())
         .arg(settings->value(QString("themes.%1.event").arg(theme)).toString())
         .arg(settings->value(QString("themes.%1.timestamp").arg(theme)).toString())
         .arg(settings->value(QString("themes.%1.link").arg(theme)).toString()));
}

void MessageView::receiveMessage(IrcMessage* message)
{
    bool ignore = false;
    MessageFormatter::Options options;

    switch (message->type()) {
        case IrcMessage::Private: {
            IrcSender sender = message->sender();
            if (sender.name() == QLatin1String("***") && sender.user() == QLatin1String("znc")) {
                QString content = static_cast<IrcPrivateMessage*>(message)->message();
                if (content == QLatin1String("Buffer Playback..."))
                    ignore = true;
                else if (content == QLatin1String("Playback Complete."))
                    ignore = true;
            }
            if (static_cast<IrcPrivateMessage*>(message)->message().contains(d.session->nickName()))
                options.highlight = true;
            break;
        }
        case IrcMessage::Notice:
            if (static_cast<IrcNoticeMessage*>(message)->message().contains(d.session->nickName()))
                options.highlight = true;
            break;
        case IrcMessage::Topic:
            d.topic = static_cast<IrcTopicMessage*>(message)->topic();
            d.topicLabel->setText(MessageFormatter::formatHtml(d.topic));
            if (d.topicLabel->text().isEmpty())
                d.topicLabel->setText(tr("-"));
            break;
        case IrcMessage::Join:
            if (!d.playback && message->flags() & IrcMessage::Own) {
                ++d.joined;
                int blocks = d.textBrowser->document()->blockCount();
                if (blocks > 1)
                    d.textBrowser->addMarker(blocks);
                emit activeChanged();
            }
            break;
        case IrcMessage::Quit:
        case IrcMessage::Part:
            if (!d.playback && message->flags() & IrcMessage::Own) {
                ++d.parted;
                emit activeChanged();
            }
            break;
        case IrcMessage::Kick:
            if (!d.playback && !static_cast<IrcKickMessage*>(message)->user().compare(d.session->nickName(), Qt::CaseInsensitive)) {
                ++d.parted;
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

    options.nickName = d.session->nickName();
    if (IrcUserModel* model = d.listView->userModel())
        options.users = model->names();
    // TODO: cache
    options.stripNicks = Application::settings()->value("format.hideUserHosts").toBool();
    options.timeStampFormat = Application::settings()->value("formatting.timeStamp").toString();
    options.textFormat = *irc_text_format();
    if (d.viewType == ViewInfo::Channel)
        options.repeat = (d.joined > 1 && d.joined > d.parted);
    else if (d.viewType == ViewInfo::Server)
        options.repeat = (d.connected > 1);

    QString formatted = MessageFormatter::formatMessage(message, options);
    if (formatted.length()) {
        if (!ignore && (!isVisible() || !isActiveWindow())) {
            IrcMessage::Type type = message->type();
            if (options.highlight || ((type == IrcMessage::Notice || type == IrcMessage::Private) && d.viewType != ViewInfo::Channel))
                emit highlighted(message);
            else if (type == IrcMessage::Notice || type == IrcMessage::Private) // TODO: || (!d.receivedCodes.contains(Irc::RPL_ENDOFMOTD) && d.viewType == ViewInfo::Server))
                emit missed(message);
        }

        d.textBrowser->append(formatted, options.highlight);
    }
}

bool MessageView::hasUser(const QString& user) const
{
    return (!d.session->nickName().compare(user, Qt::CaseInsensitive) && d.viewType != ViewInfo::Query) ||
           (d.viewType == ViewInfo::Query && !d.receiver.compare(user, Qt::CaseInsensitive)) ||
           (d.viewType == ViewInfo::Channel && d.listView->hasUser(user));
}
