/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "messageview.h"
#include "session.h"
#include "settings.h"
#include "completer.h"
#include "application.h"
#include "stringlistmodel.h"
#include <QShortcut>
#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include <irccommand.h>
#include <ircprefix.h>
#include <ircutil.h>
#include <irc.h>
#include "commandparser.h"

enum ModelRole
{
    Role_Names,
    Role_Commands,
    Role_Aliases
};

MessageView::MessageView(IrcSession* session, QWidget* parent) :
    QWidget(parent)
{
    d.setupUi(this);

    setFocusProxy(d.editFrame->lineEdit());
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);

    d.session = session;
    d.model = new StringListModel(this);
    d.formatter.setHightlights(QStringList(session->nickName()));

    /* TODO:
    QStringList commands = IrcMessage::availableCommands();
    QMutableStringListIterator it2(commands);
    while (it2.hasNext())
        it2.next().prepend("/");
    d.model->setStringList(Role_Commands, commands);*/

    d.editFrame->completer()->setModel(d.model);
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

void MessageView::showHelp(const QString& text, bool error)
{
    /* TODO:
    QString syntax;
    if (text == "/")
    {
        QStringList commands = IrcMessage::availableCommands();
        commands.sort();
        syntax = commands.join(" ");
    }
    else if (text.startsWith('/'))
    {
        QString command = text.mid(1).split(' ', QString::SkipEmptyParts).value(0).toUpper();
        IrcMessage* message = IrcMessage::create(command, this);
        if (message)
            syntax = message->syntax();
        delete message;
    }
    d.helpLabel->setVisible(!syntax.isEmpty());
    QPalette pal;
    if (error)
        pal.setColor(QPalette::WindowText, Qt::red);
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);*/
}

void MessageView::appendMessage(const QString& message)
{
    // workaround the link activation merge char format bug
//    if (message.endsWith("</a>"))
//        message += " ";

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
    // TODO: query, help, echo privmsg, check empty txt...
    IrcCommand* cmd = CommandParser::parseCommand(d.receiver, text, this);
    if (cmd)
        d.session->sendCommand(cmd);
    delete cmd;
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
    switch (message->type())
    {
    case IrcMessage::Invite:
        inviteMessage(static_cast<IrcInviteMessage*>(message));
        break;
    case IrcMessage::Join:
        joinMessage(static_cast<IrcJoinMessage*>(message));
        break;
    case IrcMessage::Kick:
        kickMessage(static_cast<IrcKickMessage*>(message));
        break;
    case IrcMessage::Mode:
        modeMessage(static_cast<IrcModeMessage*>(message));
        break;
    case IrcMessage::Nick:
        nickMessage(static_cast<IrcNickMessage*>(message));
        break;
    case IrcMessage::Notice:
        noticeMessage(static_cast<IrcNoticeMessage*>(message));
        break;
    case IrcMessage::Numeric:
        numericMessage(static_cast<IrcNumericMessage*>(message));
        break;
    case IrcMessage::Part:
        partMessage(static_cast<IrcPartMessage*>(message));
        break;
    case IrcMessage::Private:
        privateMessage(static_cast<IrcPrivateMessage*>(message));
        break;
    case IrcMessage::Quit:
        quitMessage(static_cast<IrcQuitMessage*>(message));
        break;
    case IrcMessage::Topic:
        topicMessage(static_cast<IrcTopicMessage*>(message));
        break;
    case IrcMessage::Unknown:
        unknownMessage(static_cast<IrcMessage*>(message));
        break;
    }
}

void MessageView::inviteMessage(IrcInviteMessage* message)
{
    appendMessage(d.formatter.formatMessage(message));
}

void MessageView::joinMessage(IrcJoinMessage* message)
{
    if (Application::settings().messages.value(Settings::Joins))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Joins))
        emit highlight(this, true);

    d.model->add(Role_Names, IrcPrefix(message->prefix()).nick());
}

void MessageView::kickMessage(IrcKickMessage* message)
{
    if (Application::settings().messages.value(Settings::Kicks))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Kicks))
        emit highlight(this, true);

    d.model->remove(Role_Names, message->user());
}

void MessageView::modeMessage(IrcModeMessage* message)
{
    if (Application::settings().messages.value(Settings::Modes))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Modes))
        emit highlight(this, true);
}

void MessageView::nickMessage(IrcNickMessage* message)
{
    if (Application::settings().messages.value(Settings::Nicks))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Nicks))
        emit highlight(this, true);

    d.model->remove(Role_Names, IrcPrefix(message->prefix()).nick());
    d.model->add(Role_Names, message->nick());
}

void MessageView::noticeMessage(IrcNoticeMessage* message)
{
    bool matches = message->message().contains(d.session->nickName());
    if (matches)
        emit alert(this, true);
    else
        emit highlight(this, true);

    appendMessage(d.formatter.formatMessage(message));
}

void MessageView::numericMessage(IrcNumericMessage* message)
{
    switch (message->code())
    {
        // TODO: connected
        case Irc::RPL_WELCOME:
            emit highlight(this, false);
            break;

/*        case Irc::RPL_NAMREPLY:
            if (!d.receiver.compare(params.value(1), Qt::CaseInsensitive))
            {
                QStringList nicks = params.value(2).split(" ", QString::SkipEmptyParts);
                foreach (const QString& nick, nicks)
                    d.model->add(Role_Names, nick);
            }
            return;

        case Irc::RPL_ENDOFNAMES:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
            {
                QStringList nicks = d.model->stringList(Role_Names);
                nicks.sort();
                receiveMessage(tr("! %2 on %1"), QStringList() << params.value(0) << nicks.join(", "));
            }
            return;*/

        default:
            appendMessage(d.formatter.formatMessage(message));
            break;
    }
}

void MessageView::partMessage(IrcPartMessage* message)
{
    if (Application::settings().messages.value(Settings::Parts))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Parts))
        emit highlight(this, true);

    d.model->remove(Role_Names, IrcPrefix(message->prefix()).nick());
}

void MessageView::privateMessage(IrcPrivateMessage* message)
{
    bool matches = message->message().contains(d.session->nickName());
    if (matches || !isChannelView())
        emit alert(this, true);
    else
        emit highlight(this, true);

    appendMessage(d.formatter.formatMessage(message));
}

void MessageView::quitMessage(IrcQuitMessage* message)
{
    if (Application::settings().messages.value(Settings::Quits))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Quits))
        emit highlight(this, true);

    d.model->remove(Role_Names, IrcPrefix(message->prefix()).nick());
}

void MessageView::topicMessage(IrcTopicMessage* message)
{
    if (Application::settings().messages.value(Settings::Topics))
        appendMessage(d.formatter.formatMessage(message));

    if (Application::settings().highlights.value(Settings::Topics))
        emit highlight(this, true);
}

void MessageView::unknownMessage(IrcMessage* message)
{
    qWarning() << "unknown:" << message->prefix() << message->parameters();
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
