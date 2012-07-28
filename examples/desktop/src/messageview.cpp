/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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
#include "settings.h"
#include "completer.h"
#include "application.h"
#include "usermodel.h"
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QShortcut>
#include <QKeyEvent>
#include <QDateTime>
#include <QDebug>
#include <ircutil.h>
#include <irc.h>

class SortedUserModel : public QSortFilterProxyModel
{
public:
    SortedUserModel(const QString& prefixes, UserModel* userModel = 0)
        : QSortFilterProxyModel(userModel), pfx(prefixes)
    {
        setSourceModel(userModel);
    }

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const
    {
        QString u1 = sourceModel()->data(left, Qt::DisplayRole).toString();
        QString u2 = sourceModel()->data(right, Qt::DisplayRole).toString();

        const int i1 = pfx.indexOf(u1.at(0));
        const int i2 = pfx.indexOf(u2.at(0));

        if (i1 >= 0 && i2 < 0)
            return true;
        if (i1 < 0 && i2 >= 0)
            return false;
        if (i1 >= 0 && i2 >= 0 && i1 != i2)
            return i1 < i2;

        return QString::localeAwareCompare(u1.toLower(), u2.toLower()) < 0;
    }

private:
    QString pfx;
};

QStringListModel* MessageView::MessageViewData::commandModel = 0;

MessageView::MessageView(const QString& receiver, Session* session, QWidget* parent) :
    QWidget(parent)
{
    d.setupUi(this);

    setFocusProxy(d.lineEditor);
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);

    d.formatter.setHighlights(QStringList(session->nickName()));
    d.formatter.setMessageFormat("class='message'");
    d.formatter.setEventFormat("class='event'");
    d.formatter.setNoticeFormat("class='notice'");
    d.formatter.setActionFormat("class='action'");
    d.formatter.setUnknownFormat("class='unknown'");
    d.formatter.setHighlightFormat("class='highlight'");
    d.formatter.setTimeStampFormat("class='timestamp'");

    setReceiver(receiver);
    d.session = session;
    d.userModel = new UserModel(d.session);
    connect(&d.parser, SIGNAL(customCommand(QString,QStringList)), this, SLOT(onCustomCommand(QString,QStringList)));

    bool isChannel = session->isChannel(receiver);
    d.topicLabel->setVisible(isChannel);
    d.listView->setVisible(isChannel);
    if (isChannel)
    {
        d.listView->setModel(new SortedUserModel(session->prefixModes(), d.userModel));
        connect(d.listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    }
    d.joining = false;
    d.connecting = false;

    if (!d.commandModel)
    {
        CommandParser::addCustomCommand("QUERY", "<user>");

        QStringList prefixedCommands;
        foreach (const QString& command, CommandParser::availableCommands())
            prefixedCommands += "/" + command;

        d.commandModel = new QStringListModel(qApp);
        d.commandModel->setStringList(prefixedCommands);
    }

    d.lineEditor->completer()->setDefaultModel(d.userModel);
    d.lineEditor->completer()->setSlashModel(d.commandModel);

    connect(d.lineEditor, SIGNAL(send(QString)), this, SLOT(onSend(QString)));
    connect(d.lineEditor, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

    d.helpLabel->hide();
    d.searchEditor->setTextEdit(d.textBrowser);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));
    applySettings(Application::settings());
}

MessageView::~MessageView()
{
    delete d.userModel;
}

bool MessageView::isServerView() const
{
    QTabWidget* tabWidget = 0;
    if (parentWidget())
        tabWidget = qobject_cast<QTabWidget*>(parentWidget()->parentWidget());
    return tabWidget && tabWidget->indexOf(const_cast<MessageView*>(this)) == 0;
}

bool MessageView::isChannelView() const
{
    return d.session->isChannel(receiver());
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
    if (!message.isEmpty())
    {
        // workaround the link activation merge char format bug
        QString copy = message;
        if (copy.endsWith("</a>"))
            copy += " ";

        d.textBrowser->append(copy);
        int ub = d.textBrowser->unseenBlock();
        d.textBrowser->setUnseenBlock(isVisible() ? -1 : ub != -1 ? ub : d.textBrowser->document()->blockCount() - 1);
    }
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
                    QApplication::sendEvent(d.lineEditor, keyEvent);
                    d.lineEditor->setFocus();
                    return true;
                }
                break;
        }
    }
    else if (event->type() == QEvent::Hide)
    {
        d.textBrowser->setUnseenBlock(-1);
    }
    return false;
}

void MessageView::onEscPressed()
{
    d.helpLabel->hide();
    d.searchEditor->hide();
    setFocus(Qt::OtherFocusReason);
}

void MessageView::onSend(const QString& text)
{
    IrcCommand* cmd = d.parser.parseCommand(receiver(), text);
    if (cmd)
    {
        d.session->sendCommand(cmd);

        if (cmd->type() == IrcCommand::Message || cmd->type() == IrcCommand::CtcpAction)
        {
            IrcMessage* msg = IrcMessage::fromCommand(d.session->nickName(), cmd);
            receiveMessage(msg);
            delete msg;
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
    if (!settings.font.isEmpty())
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
            ".timestamp { color: %6; font-size: small }"
        ).arg(settings.colors.value((Settings::Highlight)))
         .arg(settings.colors.value((Settings::Message)))
         .arg(settings.colors.value((Settings::Notice)))
         .arg(settings.colors.value((Settings::Action)))
         .arg(settings.colors.value((Settings::Event)))
         .arg(settings.colors.value((Settings::TimeStamp))));
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
        if (message->sender().name() == d.session->nickName())
            d.joining = true;
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
        d.topicLabel->setText(static_cast<IrcTopicMessage*>(message)->topic());
        if (d.topicLabel->text().isEmpty())
            d.topicLabel->setText(tr("-"));
        break;
    case IrcMessage::Unknown:
        qWarning() << "unknown:" << message;
        append = false;
        break;
    case IrcMessage::Invite:
    case IrcMessage::Ping:
    case IrcMessage::Pong:
    case IrcMessage::Error:
        break;
    case IrcMessage::Numeric:
        switch (static_cast<IrcNumericMessage*>(message)->code())
        {
            case Irc::RPL_WELCOME:
                d.connecting = true;
                break;
            case Irc::RPL_ENDOFMOTD:
                d.connecting = false;
                break;
            case Irc::RPL_ENDOFNAMES:
                if (d.joining)
                {
                    appendMessage(d.formatter.formatMessage(tr("! %1 has %2 users").arg(receiver()).arg(d.userModel->rowCount())));
                    d.joining = false;
                    return;
                }
                break;
            case Irc::RPL_NAMREPLY:
                if (d.joining)
                    return;
                break;
            case Irc::RPL_NOTOPIC:
                d.topicLabel->setText(tr("-"));
                if (d.joining)
                    return;
                break;
            case Irc::RPL_TOPIC:
                d.topicLabel->setText(IrcUtil::messageToHtml(message->parameters().value(2)));
                if (d.joining)
                    return;
                break;
            case Irc::RPL_TOPICWHOTIME: {
                QDateTime dateTime = QDateTime::fromTime_t(message->parameters().value(3).toInt());
                d.topicLabel->setToolTip(tr("Set %1 by %2").arg(dateTime.toString(), message->parameters().value(2)));
                if (d.joining)
                    return;
                break;
            }
            default:
                break;
        }
        break;
    }

    QString formatted = d.formatter.formatMessage(message);
    if (append && formatted.length())
    {
        if (matches)
            emit alert(this, true);
        else if (hilite || (!d.connecting && isServerView()))
            emit highlight(this, true);

        appendMessage(formatted);
    }
}

bool MessageView::hasUser(const QString& user) const
{
    return d.userModel->hasUser(user);
}

void MessageView::addUser(const QString& user)
{
    d.userModel->addUser(user);
    d.listView->model()->sort(0);
}

void MessageView::addUsers(const QStringList& users)
{
    d.userModel->addUsers(users);
    d.listView->model()->sort(0);
}

void MessageView::removeUser(const QString& user)
{
    d.userModel->removeUser(user);
}

void MessageView::clearUsers()
{
    d.userModel->clearUsers();
}

void MessageView::renameUser(const QString &from, const QString &to)
{
    d.userModel->renameUser(from, to);
    d.listView->model()->sort(0);
}

void MessageView::setUserMode(const QString& user, const QString& mode)
{
    d.userModel->setUserMode(user, mode);
}

void MessageView::onCustomCommand(const QString& command, const QStringList& params)
{
    if (command == "QUERY")
        emit query(params.value(0));
}

void MessageView::onDoubleClicked(const QModelIndex& index)
{
    emit query(index.data(Qt::EditRole).toString());
}
