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
#include "completer.h"
#include "usermodel.h"
#include "sortedusermodel.h"
#include "session.h"
#include <QStringListModel>
#include <QTextBlock>
#include <QShortcut>
#include <QKeyEvent>
#include <QDateTime>
#include <QDebug>
#include <ircmessage.h>
#include <irccommand.h>
#include <ircutil.h>
#include <irc.h>

static QStringListModel* command_model = 0;

MessageView::MessageView(MessageView::ViewType type, Session* session, QWidget* parent) :
    QWidget(parent)
{
    d.setupUi(this);
    d.viewType = type;

    setFocusProxy(d.lineEditor);
    d.textBrowser->setBuddy(d.lineEditor);

    d.formatter.setHighlights(QStringList(session->nickName()));
    d.formatter.setMessageFormat("class='message'");
    d.formatter.setEventFormat("class='event'");
    d.formatter.setNoticeFormat("class='notice'");
    d.formatter.setActionFormat("class='action'");
    d.formatter.setUnknownFormat("class='unknown'");
    d.formatter.setHighlightFormat("class='highlight'");
    d.formatter.setTimeStampFormat("class='timestamp'");

    d.session = session;
    d.userModel = new UserModel(d.session);
    connect(&d.parser, SIGNAL(customCommand(QString,QStringList)), this, SLOT(onCustomCommand(QString,QStringList)));

    d.topicLabel->setVisible(type == ChannelView);
    d.listView->setVisible(type == ChannelView);
    if (type == ChannelView)
    {
        d.listView->setModel(new SortedUserModel(session->prefixModes(), d.userModel));
        connect(d.listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    }

    if (!command_model)
    {
        CommandParser::addCustomCommand("QUERY", "<user>");

        QStringList prefixedCommands;
        foreach (const QString& command, CommandParser::availableCommands())
            prefixedCommands += "/" + command;

        command_model = new QStringListModel(qApp);
        command_model->setStringList(prefixedCommands);
    }

    d.lineEditor->completer()->setDefaultModel(d.userModel);
    d.lineEditor->completer()->setSlashModel(command_model);

    connect(d.lineEditor, SIGNAL(send(QString)), this, SLOT(onSend(QString)));
    connect(d.lineEditor, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

    d.helpLabel->hide();
    d.searchEditor->setTextEdit(d.textBrowser);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    applySettings(d.settings);
}

MessageView::~MessageView()
{
    delete d.userModel;
}

MessageView::ViewType MessageView::viewType() const
{
    return d.viewType;
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
        pal.setColor(QPalette::WindowText, d.settings.colors[Settings::Highlight]);
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
        if (!isVisible() && d.textBrowser->unseenBlock() == -1)
            d.textBrowser->setUnseenBlock(d.textBrowser->document()->blockCount() - 1);

#if QT_VERSION >= 0x040800
        QTextBlock block = d.textBrowser->document()->lastBlock();
        QTextBlockFormat format = block.blockFormat();
        format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
        QTextCursor cursor(block);
        cursor.setBlockFormat(format);
#endif // QT_VERSION
    }
}

void MessageView::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    d.textBrowser->setUnseenBlock(-1);
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
            IrcMessage* msg = IrcMessage::fromCommand(d.session->nickName(), cmd, d.session);
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
    d.settings = settings;
}

void MessageView::receiveMessage(IrcMessage* message)
{
    if (d.userModel)
        d.userModel->processMessage(message, receiver());

    bool append = true;
    bool hilite = false;
    bool matches = false;

    switch (message->type())
    {
    case IrcMessage::Join:
        append = d.settings.messages.value(Settings::Joins);
        hilite = d.settings.highlights.value(Settings::Joins);
        break;
    case IrcMessage::Kick:
        append = d.settings.messages.value(Settings::Kicks);
        hilite = d.settings.highlights.value(Settings::Kicks);
        break;
    case IrcMessage::Mode:
        append = d.settings.messages.value(Settings::Modes);
        hilite = d.settings.highlights.value(Settings::Modes);
        break;
    case IrcMessage::Nick:
        append = d.settings.messages.value(Settings::Nicks);
        hilite = d.settings.highlights.value(Settings::Nicks);
        break;
    case IrcMessage::Notice:
        matches = static_cast<IrcNoticeMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Part:
        append = d.settings.messages.value(Settings::Parts);
        hilite = d.settings.highlights.value(Settings::Parts);
        break;
    case IrcMessage::Private:
        matches = d.viewType != ChannelView || static_cast<IrcPrivateMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Quit:
        append = d.settings.messages.value(Settings::Quits);
        hilite = d.settings.highlights.value(Settings::Quits);
        break;
    case IrcMessage::Topic:
        append = d.settings.messages.value(Settings::Topics);
        hilite = d.settings.highlights.value(Settings::Topics);
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
            case Irc::RPL_NOTOPIC:
                d.topicLabel->setText(tr("-"));
                break;
            case Irc::RPL_TOPIC:
                d.topicLabel->setText(IrcUtil::messageToHtml(message->parameters().value(2)));
                break;
            case Irc::RPL_TOPICWHOTIME: {
                QDateTime dateTime = QDateTime::fromTime_t(message->parameters().value(3).toInt());
                d.topicLabel->setToolTip(tr("Set %1 by %2").arg(dateTime.toString(), message->parameters().value(2)));
                break;
            }
            default:
                break;
        }
        break;
    }

    QString formatted = d.formatter.formatMessage(message, d.userModel);
    if (append && formatted.length())
    {
        if (matches)
            emit alerted(message);
        else if (hilite) // TODO: || (!d.receivedCodes.contains(Irc::RPL_ENDOFMOTD) && d.viewType == ServerView))
            emit highlighted(message);

        appendMessage(formatted);
    }
}

bool MessageView::hasUser(const QString& user) const
{
    return d.userModel->hasUser(user);
}

void MessageView::onCustomCommand(const QString& command, const QStringList& params)
{
    if (command == "QUERY")
        emit queried(params.value(0));
}

void MessageView::onDoubleClicked(const QModelIndex& index)
{
    emit queried(index.data(Qt::EditRole).toString());
}
