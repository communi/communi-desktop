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
#include <ircprefix.h>
#include <ircutil.h>
#include <irc.h>

enum ModelRole
{
    Role_Names,
    Role_Commands,
    Role_Aliases
};

static const QStringList NICK_COLORS = QStringList()
    << QLatin1String("#BF0000")
    << QLatin1String("#FF0000")
    << QLatin1String("#BF7900")
    << QLatin1String("#00BC0F")
    << QLatin1String("#00A7BA")
    << QLatin1String("#0F00B7")
    << QLatin1String("#8100B5");

MessageView::MessageView(IrcSession* session, QWidget* parent) :
    QWidget(parent), IrcReceiver(session)
{
    d.setupUi(this);

    setFocusProxy(d.editFrame->lineEdit());
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);

    d.model = new StringListModel(this);

    QStringList commands = IrcMessage::availableCommands();
    QMutableStringListIterator it2(commands);
    while (it2.hasNext())
        it2.next().prepend("/");
    d.model->setStringList(Role_Commands, commands);

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
    d.helpLabel->setText(syntax);
}

void MessageView::receiveMessage(const QString& format, const QStringList& params, bool highlight)
{
    QString message = format;
    foreach (const QString& param, params)
        message = message.arg(param);

    // workaround the link activation merge char format bug
    if (message.endsWith("</a>"))
        message += " ";

    QString cls = "message";
    if (highlight)
        cls = "highlight";
    else if (message.startsWith("!"))
        cls = "event";
    else if (message.startsWith("?"))
        cls = "notice";
    else if (message.startsWith("["))
        cls = "notice";
    else if (message.startsWith("*"))
        cls = "action";

    if (d.timeStamp)
    {
        const QString time = QTime::currentTime().toString();
        message = tr("[%1] %2").arg(time).arg(message);
    }
    message = QString("<span class='%1'>%2</span>").arg(cls).arg(message);
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
    if (text.startsWith('/'))
    {
        QStringList words = text.mid(1).split(" ");
        IrcMessage* msg = IrcMessage::create(words.value(0).toUpper());
        if (msg && msg->initFrom(session()->nickName(), words.mid(1)))
        {
            session()->sendMessage(msg);
            if (IrcSendMessage* sendMessage = qobject_cast<IrcSendMessage*>(msg))
                receiveMessage(sendMessage);
            msg->deleteLater();
        }
        else
        {
            showHelp(text, true);
        }
    }
    else if (!text.trimmed().isEmpty())
    {
        IrcPrivateMessage msg;
        msg.setPrefix(session()->nickName());
        msg.setTarget(d.receiver);
        msg.setMessage(text);
        session()->sendMessage(&msg);
        receiveMessage(&msg);
    }
}

void MessageView::applySettings(const Settings& settings)
{
    d.timeStamp = settings.timeStamp;
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
    const QString lower = d.receiver.toLower();
    if (IrcChannelMessage* chanMsg = qobject_cast<IrcChannelMessage*>(message))
    {
        if (chanMsg->channel() != lower)
            return;
    }
    if (IrcModeMessage* modeMsg = qobject_cast<IrcModeMessage*>(message))
    {
        if (modeMsg->target().toLower() != lower)
            return;
    }
    if (IrcSendMessage* sendMsg = qobject_cast<IrcSendMessage*>(message))
    {
        if (sendMsg->target().toLower() != lower)
            return;
    }
    if (IrcQueryMessage* queryMsg = qobject_cast<IrcQueryMessage*>(message))
    {
        Q_UNUSED(queryMsg);
        if (!isCurrentView())
            return;
    }
    if (IrcErrorMessage* errorMsg = qobject_cast<IrcErrorMessage*>(message))
    {
        Q_UNUSED(errorMsg);
        if (!isCurrentView())
            return;
    }
    if (IrcQuitMessage* quitMsg = qobject_cast<IrcQuitMessage*>(message))
    {
        if (!d.model->stringList(Role_Names).contains(IrcPrefix(quitMsg->prefix()).nick(), Qt::CaseInsensitive))
            return;
    }
    if (IrcNickMessage* nickMsg = qobject_cast<IrcNickMessage*>(message))
    {
        if (!d.model->stringList(Role_Names).contains(IrcPrefix(nickMsg->prefix()).nick(), Qt::CaseInsensitive))
            return;
    }
    IrcReceiver::receiveMessage(message);
}

void MessageView::inviteMessage(IrcInviteMessage* message)
{
    QStringList params;
    params << prettyUser(message->prefix()) << message->channel();
    receiveMessage(tr("! %1 invited to %3"), params);
}

void MessageView::joinMessage(IrcJoinMessage* message)
{
    if (Application::settings().messages.value(Settings::Joins))
    {
        QStringList params;
        params << prettyUser(message->prefix()) << message->channel();
        receiveMessage(tr("! %1 joined %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Joins))
        emit highlight(this, true);

    d.model->add(Role_Names, IrcPrefix(message->prefix()).nick());
}

void MessageView::kickMessage(IrcKickMessage* message)
{
    if (Application::settings().messages.value(Settings::Kicks))
    {
        QStringList params;
        params << prettyUser(message->prefix()) << prettyUser(message->user());
        if (!message->reason().isEmpty())
            receiveMessage(tr("! %1 kicked %2 (%3)"), params << message->reason());
        else
            receiveMessage(tr("! %1 kicked %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Kicks))
        emit highlight(this, true);

    d.model->remove(Role_Names, message->user());
}

void MessageView::modeMessage(IrcModeMessage* message)
{
    if (Application::settings().messages.value(Settings::Modes))
    {
        QStringList params;
        params << prettyUser(message->prefix()) << message->mode() << message->argument();
        receiveMessage(tr("! %1 sets mode %2 %3"), params);
    }
    if (Application::settings().highlights.value(Settings::Modes))
        emit highlight(this, true);
}

void MessageView::nickNameMessage(IrcNickMessage* message)
{
    if (Application::settings().messages.value(Settings::Nicks))
    {
        QStringList params;
        params << prettyUser(message->prefix()) << prettyUser(message->nick());
        receiveMessage(tr("! %1 changed nick to %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Nicks))
        emit highlight(this, true);

    d.model->remove(Role_Names, IrcPrefix(message->prefix()).nick());
    d.model->add(Role_Names, message->nick());
}

void MessageView::noticeMessage(IrcNoticeMessage* message)
{
    bool matches = message->message().contains(session()->nickName());
    if (matches)
        emit alert(this, true);
    else
        emit highlight(this, true);

    QStringList params;
    params << prettyUser(message->prefix()) << IrcUtil::messageToHtml(message->message());
    receiveMessage(tr("[%1] %2"), params, matches);
}

void MessageView::numericMessage(IrcNumericMessage* message)
{
    const QStringList params = message->parameters();
    switch (message->code())
    {
        case Irc::RPL_WELCOME:
            emit highlight(this, false);
            break;

        case Irc::RPL_AWAY:
        case Irc::RPL_WHOISOPERATOR:
        case 310: // "is available for help"
        case 320: // "is identified to services"
        case 378: // nick is connecting from <...>
        case 671: // nick is using a secure connection
            if (isCurrentView())
            {
                QStringList list;
                list << prettyUser(params.value(0)) << params.at(1);
                receiveMessage("! %1 %2", list);
            }
            return;

        case Irc::RPL_WHOISUSER:
            if (isCurrentView())
            {
                QStringList list;
                list << prettyUser(params.value(0)) << params.value(1) << params.value(2) << params.value(3);
                receiveMessage(tr("! %1 is %2@%3 %4"), list);
            }
            return;

        case Irc::RPL_WHOISSERVER:
            if (isCurrentView())
            {
                QStringList list;
                list << prettyUser(params.value(0)) << params.value(1) << params.value(2);
                receiveMessage(tr("! %1 online via %2 (%3)"), list);
            }
            return;

        case 330: // nick user is logged in as
            if (isCurrentView())
            {
                QStringList list;
                list << prettyUser(params.value(0)) << params.value(1) << params.value(2);
                receiveMessage(tr("! %1 %3 %2"), list);
            }
            return;

        case Irc::RPL_WHOWASUSER:
            if (isCurrentView())
                receiveMessage(tr("! %1 was %2@%3 %4 %5"), params);
            return;

        case Irc::RPL_ENDOFWHOWAS:
            // End of /WHOWAS list.
            return;

        case Irc::RPL_WHOISIDLE:
            if (isCurrentView())
            {
                QString sender = prettyUser(params.value(0));

                QTime idle = QTime().addSecs(params.value(1).toInt());
                receiveMessage(tr("! %1 has been idle for %2"), QStringList() << sender << idle.toString());

                QDateTime signon = QDateTime::fromTime_t(params.value(2).toInt());
                receiveMessage(tr("! %1 has been online since %2"), QStringList() << sender << signon.toString());
            }
            return;

        case Irc::RPL_ENDOFWHOIS:
            // End of /WHOIS list.
            return;

        case Irc::RPL_WHOISCHANNELS:
            if (isCurrentView())
            {
                QStringList list;
                list << prettyUser(params.value(0)) << params.value(1);
                receiveMessage(tr("! %1 is on channels %2"), list);
            }
            return;

        case Irc::RPL_CHANNELMODEIS:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
                receiveMessage(tr("! %1 mode is %2"), QStringList() << params.value(0) << params.value(1));
            return;

        case Irc::RPL_CHANNELURL:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
                receiveMessage(tr("! %1 url is %2"), QStringList() << params.value(0) << params.value(1));
            return;

        case Irc::RPL_CHANNELCREATED:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
            {
                QDateTime dateTime = QDateTime::fromTime_t(params.value(1).toInt());
                receiveMessage(tr("! %1 was created %2"), QStringList() << params.value(0) << dateTime.toString());
            }
            return;

        case Irc::RPL_NOTOPIC:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
                receiveMessage(tr("! %1 has no topic set"), QStringList() << params.value(0));
            return;

        case Irc::RPL_TOPIC:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
                receiveMessage(tr("! %1 topic is \"%2\""), QStringList() << params.value(0) << IrcUtil::messageToHtml(params.value(1)));
            return;

        case Irc::RPL_TOPICSET:
            if (!d.receiver.compare(params.value(0), Qt::CaseInsensitive))
            {
                QDateTime dateTime = QDateTime::fromTime_t(params.value(2).toInt());
                if (dateTime.isValid())
                {
                    QStringList list;
                    list << params.value(0) << dateTime.toString() << prettyUser(params.value(1));
                    receiveMessage(tr("! %1 topic set %2 by %3"), list);
                }
            }
            return;

        case Irc::RPL_INVITING:
            if (isCurrentView())
                receiveMessage(tr("! inviting %1 to %2"), QStringList() << params.value(0) << params.value(1));
            return;

        case Irc::RPL_VERSION:
            if (isCurrentView())
                receiveMessage(tr("! %1 version is %2"), QStringList() << message->prefix() << params.value(0));
            return;

        case Irc::RPL_NAMREPLY:
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
            return;

        case Irc::RPL_TIME:
            if (isCurrentView())
                receiveMessage(tr("! %1 time is %2"), QStringList() << params.value(0) << params.value(1));
            return;

        default:
            if (isCurrentView())
                receiveMessage(tr("[%1] %2"), QStringList() << QString::number(message->code()) << params.join(" "));
            break;
    }
}

void MessageView::partMessage(IrcPartMessage* message)
{
    if (Application::settings().messages.value(Settings::Parts))
    {
        QStringList params;
        params << prettyUser(message->prefix()) << message->channel();
        if (!message->reason().isEmpty())
            receiveMessage(tr("! %1 parted %2 (%3)"), params << message->reason());
        else
            receiveMessage(tr("! %1 parted %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Parts))
        emit highlight(this, true);

    d.model->remove(Role_Names, IrcPrefix(message->prefix()).nick());
}

void MessageView::privateMessage(IrcPrivateMessage* message)
{
    bool matches = message->message().contains(session()->nickName());
    bool privmsg = !Session::isChannel(d.receiver);
    if (matches || privmsg)
        emit alert(this, true);
    else
        emit highlight(this, true);

    QStringList params;
    params << prettyUser(message->prefix()) << IrcUtil::messageToHtml(message->message());
    if (message->isAction())
        receiveMessage(tr("* %1 %2"), params, matches);
    else
        receiveMessage(tr("&lt;%1&gt; %2"), params, matches);
}

void MessageView::quitMessage(IrcQuitMessage* message)
{
    if (Application::settings().messages.value(Settings::Quits))
    {
        QStringList params;
        params << prettyUser(message->prefix());
        if (!message->reason().isEmpty())
            receiveMessage(tr("! %1 has quit (%2)"), params << message->reason());
        else
            receiveMessage(tr("! %1 has quit"), params);
    }
    if (Application::settings().highlights.value(Settings::Quits))
        emit highlight(this, true);

    d.model->remove(Role_Names, IrcPrefix(message->prefix()).nick());
}

void MessageView::topicMessage(IrcTopicMessage* message)
{
    if (Application::settings().messages.value(Settings::Topics))
    {
        QStringList params;
        params << prettyUser(message->prefix()) << IrcUtil::messageToHtml(message->topic()) << message->channel();
        receiveMessage(tr("! %1 sets topic \"%2\" on %3"), params);
    }
    if (Application::settings().highlights.value(Settings::Topics))
        emit highlight(this, true);
}

void MessageView::unknownMessage(IrcMessage* message)
{
    qWarning() << "unknown:" << message->prefix() << message->parameters();
}

QString MessageView::prettyUser(const QString& user) const
{
    QString nick = IrcPrefix(user).nick();
    QString color = NICK_COLORS.at(qHash(nick) % NICK_COLORS.count());
    return QString("<span style='color:%1'>%2</span>").arg(color).arg(nick);
}

bool MessageView::isCurrentView() const
{
    QTabWidget* tabWidget = 0;
    if (parentWidget())
        tabWidget = qobject_cast<QTabWidget*>(parentWidget()->parentWidget());
    return tabWidget && tabWidget->currentWidget() == this;
}
