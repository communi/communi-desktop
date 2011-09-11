/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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
#include "session.h"
#include "settings.h"
#include "completer.h"
#include "application.h"
#include <QStringListModel>
#include <QShortcut>
#include <QKeyEvent>
#include <QDebug>
#include <irccommand.h>
#include <ircutil.h>
#include <irc.h>

QStringListModel* MessageView::MessageViewData::commandModel = 0;

MessageView::MessageView(IrcSession* session, QWidget* parent) :
    QWidget(parent)
{
    d.setupUi(this);

    setFocusProxy(d.editFrame->lineEdit());
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);

    d.session = session;
    d.formatter.setHightlights(QStringList(session->nickName()));
    connect(&d.parser, SIGNAL(customCommand(QString,QStringList)), this, SLOT(onCustomCommand(QString,QStringList)));

    d.userModel = new QStringListModel(this);

    if (!d.commandModel)
    {
        CommandParser::addCustomCommand("CONNECT", "(<host> <port>)");
        CommandParser::addCustomCommand("QUERY", "<user>");
        CommandParser::addCustomCommand("SETTINGS", "");

        QStringList prefixedCommands;
        foreach (const QString& command, CommandParser::availableCommands())
            prefixedCommands += "/" + command;

        d.commandModel = new QStringListModel(qApp);
        d.commandModel->setStringList(prefixedCommands);
    }

    d.editFrame->completer()->setDefaultModel(d.userModel);
    d.editFrame->completer()->setSlashModel(d.commandModel);

    connect(d.editFrame, SIGNAL(send(QString)), this, SLOT(onSend(QString)));
    connect(d.editFrame, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

    d.helpLabel->hide();
    d.findFrame->setTextEdit(d.textBrowser);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));
    applySettings(Application::settings());
}

MessageView::~MessageView()
{
}

QString MessageView::receiver() const
{
    return d.receiver;
}

void MessageView::setReceiver(const QString& receiver)
{
    d.receiver = receiver;
}

bool MessageView::isChannelView() const
{
    if (d.receiver.isEmpty())
        return false;

    switch (d.receiver.at(0).unicode())
    {
        case '#':
        case '&':
        case '!':
        case '+':
            return true;
        default:
            return false;
    }
}

void MessageView::showHelp(const QString& text, bool error)
{
    QString syntax;
    if (text == "/")
    {
        QStringList commands = CommandParser::availableCommands();
        syntax = commands.join(" ");
    }
    else if (text.startsWith('/'))
    {
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
        pal.setColor(QPalette::WindowText, Qt::red);
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);
}

void MessageView::appendMessage(const QString& message)
{
    // workaround the link activation merge char format bug
//    if (message.endsWith("</a>"))
//        message += " ";

    if (!message.isEmpty())
        d.textBrowser->append(message);
}

bool MessageView::eventFilter(QObject* receiver, QEvent* event)
{
    Q_UNUSED(receiver);
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        // for example:
        // - Ctrl+C goes to the browser
        // - Ctrl+V goes to the line edit
        // - Shift+7 ("/") goes to the line edit
        switch (keyEvent->key())
        {
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Meta:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
                break;
            default:
                if (!keyEvent->matches(QKeySequence::Copy))
                {
                    QApplication::sendEvent(d.editFrame->lineEdit(), keyEvent);
                    d.editFrame->lineEdit()->setFocus();
                    return true;
                }
                break;
        }
    }
    return false;
}

void MessageView::onEscPressed()
{
    d.helpLabel->hide();
    d.findFrame->hide();
    setFocus(Qt::OtherFocusReason);
}

void MessageView::onSend(const QString& text)
{
    IrcCommand* cmd = d.parser.parseCommand(d.receiver, text);
    if (cmd)
    {
        d.session->sendCommand(cmd);

        if (cmd->type() == IrcCommand::Message)
        {
            IrcPrivateMessage msg;
            msg.initFrom(d.session->nickName(), cmd->parameters());
            receiveMessage(&msg);
        }
    }
    else if (d.parser.hasError())
    {
        showHelp(text, true);
    }
}

void MessageView::applySettings(const Settings& settings)
{
    d.formatter.setTimeStamp(settings.timeStamp);
    d.textBrowser->setFont(settings.font);
    d.textBrowser->document()->setMaximumBlockCount(settings.maxBlockCount);

    QString backgroundColor = settings.colors.value(Settings::Background);
    d.textBrowser->setStyleSheet(QString("QTextBrowser { background-color: %1 }").arg(backgroundColor));

    d.textBrowser->document()->setDefaultStyleSheet(
        QString(
            ".highlight { color: %1 }"
            ".message   { color: %2 }"
            ".notice    { color: %3 }"
            ".action    { color: %4 }"
            ".event     { color: %5 }"
        ).arg(settings.colors.value((Settings::Highlight)))
         .arg(settings.colors.value((Settings::Message)))
         .arg(settings.colors.value((Settings::Notice)))
         .arg(settings.colors.value((Settings::Action)))
         .arg(settings.colors.value((Settings::Event))));
}

void MessageView::receiveMessage(IrcMessage* message)
{
    bool append = true;
    bool hilite = false;
    bool matches = false;

    switch (message->type())
    {
    case IrcMessage::Join:
        append = Application::settings().messages.value(Settings::Joins);
        hilite = Application::settings().highlights.value(Settings::Joins);
        break;
    case IrcMessage::Kick:
        append = Application::settings().messages.value(Settings::Kicks);
        hilite = Application::settings().highlights.value(Settings::Kicks);
        break;
    case IrcMessage::Mode:
        append = Application::settings().messages.value(Settings::Modes);
        hilite = Application::settings().highlights.value(Settings::Modes);
        break;
    case IrcMessage::Nick:
        append = Application::settings().messages.value(Settings::Nicks);
        hilite = Application::settings().highlights.value(Settings::Nicks);
        break;
    case IrcMessage::Notice:
        matches = static_cast<IrcNoticeMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Part:
        append = Application::settings().messages.value(Settings::Parts);
        hilite = Application::settings().highlights.value(Settings::Parts);
        break;
    case IrcMessage::Private:
        matches = !isChannelView() || static_cast<IrcPrivateMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Quit:
        append = Application::settings().messages.value(Settings::Quits);
        hilite = Application::settings().highlights.value(Settings::Quits);
        break;
    case IrcMessage::Topic:
        append = Application::settings().messages.value(Settings::Topics);
        hilite = Application::settings().highlights.value(Settings::Topics);
        break;
    case IrcMessage::Unknown:
        qWarning() << "unknown:" << message;
        append = false;
        break;
    case IrcMessage::Invite:
    case IrcMessage::Numeric:
    case IrcMessage::Ping:
    case IrcMessage::Pong:
    case IrcMessage::Error:
        break;
    }

    if (matches)
        emit alert(this, true);
    else if (hilite)
        emit highlight(this, true);
    if (append)
        appendMessage(d.formatter.formatMessage(message));
}

void MessageView::addUser(const QString& user)
{
    // TODO: this is far from optimal
    QStringList users = d.userModel->stringList();
    users.append(user);
    d.userModel->setStringList(users);
}

void MessageView::removeUser(const QString& user)
{
    // TODO: this is far from optimal
    QStringList users = d.userModel->stringList();
    if (users.removeOne(user))
        d.userModel->setStringList(users);
}

void MessageView::onCustomCommand(const QString& command, const QStringList& params)
{
    if (command == "QUERY")
        emit query(params.value(0));
    else if (command == "SETTINGS")
        Application::showSettings();
    else if (command == "CONNECT")
        QMetaObject::invokeMethod(window(), "connectTo", Q_ARG(QString, params.value(0)), params.count() > 1 ? Q_ARG(quint16, params.value(1).toInt()) : QGenericArgument());
}
