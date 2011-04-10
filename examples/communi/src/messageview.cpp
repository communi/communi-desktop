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
#include "menu.h"
#include "session.h"
#include "settings.h"
#include "completer.h"
#include "application.h"
#include "stringlistmodel.h"
#include "nickhighlighter.h"
#include <QDesktopServices>
#include <QStringListModel>
#include <QScrollBar>
#include <QTreeView>
#include <QShortcut>
#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include <QMenu>
#include <ircutil.h>
#include <irc.h>

enum ModelRole
{
    Role_Names,
    Role_Commands,
    Role_Aliases
};

MessageView::MessageView(/*Irc::Buffer* buffer,*/ QWidget* parent)
    : QWidget(parent)
{
    d.setupUi(this);

    //d.buffer = 0;
    //setBuffer(buffer);

    setFocusProxy(d.editFrame->lineEdit());
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);
    connect(d.textBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));

    d.model = new StringListModel(this);

    QStringList  commands = Application::commands().keys();
    QMutableStringListIterator it2(commands);
    while (it2.hasNext())
        it2.next().prepend("/");
    d.model->setStringList(Role_Commands, commands);

    QStringList aliases = Application::settings().aliases.keys();
    QMutableStringListIterator it1(aliases);
    while (it1.hasNext())
        it1.next().prepend("/");
    d.model->setStringList(Role_Aliases, aliases);

    d.editFrame->completer()->setModel(d.model);
    connect(d.editFrame, SIGNAL(send(QString)), this, SLOT(onSend(QString)));
    connect(d.editFrame, SIGNAL(help(QString)), this, SLOT(showHelp(QString)));

    d.findFrame->setTextEdit(d.textBrowser);
    (void) new NickHighlighter(d.textBrowser->document());

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));
    applySettings(Application::settings());
}

MessageView::~MessageView()
{
    //TODO: d.buffer->deleteLater();
}

/*
Irc::Buffer* MessageView::buffer() const
{
    return d.buffer;
}

void MessageView::setBuffer(Irc::Buffer* buffer)
{
    if (d.buffer != buffer)
    {
        if (d.buffer)
            d.buffer->disconnect(this);

        d.buffer = buffer;

        if (buffer)
        {
            connect(buffer, SIGNAL(receiverChanged(QString)), this, SLOT(receiverChanged()));
            connect(buffer, SIGNAL(joined(QString)), this, SLOT(msgJoined(QString)));
            connect(buffer, SIGNAL(parted(QString, QString)), this, SLOT(msgParted(QString, QString)));
            connect(buffer, SIGNAL(quit(QString, QString)), this, SLOT(msgQuit(QString, QString)));
            connect(buffer, SIGNAL(nickChanged(QString, QString)), this, SLOT(msgNickChanged(QString, QString)));
            connect(buffer, SIGNAL(modeChanged(QString, QString, QString)), this, SLOT(msgModeChanged(QString, QString, QString)));
            connect(buffer, SIGNAL(topicChanged(QString, QString)), this, SLOT(msgTopicChanged(QString, QString)));
            connect(buffer, SIGNAL(invited(QString, QString, QString)), this, SLOT(msgInvited(QString, QString, QString)));
            connect(buffer, SIGNAL(kicked(QString, QString, QString)), this, SLOT(msgKicked(QString, QString, QString)));
            connect(buffer, SIGNAL(messageReceived(QString, QString)), this, SLOT(msgMessageReceived(QString, QString)));
            connect(buffer, SIGNAL(noticeReceived(QString, QString)), this, SLOT(msgNoticeReceived(QString, QString)));
            connect(buffer, SIGNAL(ctcpRequestReceived(QString, QString)), this, SLOT(msgCtcpRequestReceived(QString, QString)));
            connect(buffer, SIGNAL(ctcpReplyReceived(QString, QString)), this, SLOT(msgCtcpReplyReceived(QString, QString)));
            connect(buffer, SIGNAL(ctcpActionReceived(QString, QString)), this, SLOT(msgCtcpActionReceived(QString, QString)));
            connect(buffer, SIGNAL(numericMessageReceived(QString, uint, QStringList)), this, SLOT(msgNumericMessageReceived(QString, uint, QStringList)));
            connect(buffer, SIGNAL(unknownMessageReceived(QString, QStringList)), this, SLOT(msgUnknownMessageReceived(QString, QStringList)));
        }
    }
}
*/

void MessageView::clear()
{
    d.textBrowser->clear();
}

void MessageView::showHelp(const QString& text)
{
    if (text.isNull())
    {
        QMapIterator<QString, Command> it(Application::commands());
        while (it.hasNext())
        {
            Command command = it.next().value();
            QStringList params;
            params << command.name.toUpper()
                   << IrcUtil::messageToHtml(command.help)
                   << IrcUtil::messageToHtml(command.description);
            receiveMessage(tr("? %1 %2 - %3"), params);
        }
    }
    else
    {
        Command command = Application::commands().value(text);
        if (command.name.isNull()) {
            receiveMessage(tr("? unknown command %1"), QStringList() << text);
        } else {
            QStringList params;
            params << command.name.toUpper()
                   << IrcUtil::messageToHtml(command.help)
                   << IrcUtil::messageToHtml(command.description);
            receiveMessage(tr("? %1 %2 - %3"), params);
        }
    }
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
    else if (event->type() == QEvent::ContextMenu)
    {
        QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(event);
        QString anchor = d.textBrowser->anchorAt(contextMenuEvent->pos());
        if (anchor.startsWith("query:"))
        {
            /* TODO:
            QString nick = QUrl(anchor).path();
            Menu* menu = Menu::createNickContextMenu(nick, d.buffer->receiver(), this);
            connect(menu, SIGNAL(command(QString)), d.buffer->session(), SLOT(raw(QString)));
            menu->exec(contextMenuEvent->globalPos());
            delete menu;
            */
            return true;
        }
    }
    return false;
}

void MessageView::onEscPressed()
{
    if (d.findFrame->isVisible())
    {
        d.findFrame->hide();
        setFocus(Qt::OtherFocusReason);
    }
}

void MessageView::onSend(const QString& text)
{
    QApplication::processEvents();
    //TODO: emit send(d.buffer->receiver(), text);
}

void MessageView::onAnchorClicked(const QUrl& link)
{
    if (link.scheme() == "query")
        emit query(link.path());
    else
        QDesktopServices::openUrl(link);
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

void MessageView::receiverChanged()
{
    emit rename(this);
}

void MessageView::msgJoined(const QString& origin)
{
    if (Application::settings().messages.value(Settings::Joins))
    {
        QStringList params;
        //TODO: params << senderHtml(origin) << d.buffer->receiver();
        receiveMessage(tr("! %1 joined %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Joins))
        emit highlight(this, true);

    //TODO: d.model->setStringList(Role_Names, d.buffer->names());
}

void MessageView::msgParted(const QString& origin, const QString& message)
{
    if (Application::settings().messages.value(Settings::Parts))
    {
        QStringList params;
        //TODO: params << senderHtml(origin) << d.buffer->receiver();
        if (!message.isEmpty())
            receiveMessage(tr("! %1 parted %2 (%3)"), params << message);
        else
            receiveMessage(tr("! %1 parted %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Parts))
        emit highlight(this, true);

    //TODO: d.model->setStringList(Role_Names, d.buffer->names());
}

void MessageView::msgQuit(const QString& origin, const QString& message)
{
    if (Application::settings().messages.value(Settings::Quits))
    {
        QStringList params;
        params << prefixedSender(origin);
        if (!message.isEmpty())
            receiveMessage(tr("! %1 has quit (%2)"), params << message);
        else
            receiveMessage(tr("! %1 has quit"), params);
    }
    if (Application::settings().highlights.value(Settings::Quits))
        emit highlight(this, true);

    //TODO: d.model->setStringList(Role_Names, d.buffer->names());
}

void MessageView::msgNickChanged(const QString& origin, const QString& nick)
{
    if (Application::settings().messages.value(Settings::Nicks))
    {
        QStringList params;
        params << prefixedSender(origin) << senderHtml(nick);
        receiveMessage(tr("! %1 changed nick to %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Nicks))
        emit highlight(this, true);

    //TODO: d.model->setStringList(Role_Names, d.buffer->names());
}

void MessageView::msgModeChanged(const QString& origin, const QString& mode, const QString& args)
{
    if (Application::settings().messages.value(Settings::Modes))
    {
        QStringList params;
        params << senderHtml(origin) << mode << args;
        receiveMessage(tr("! %1 sets mode %2 %3"), params);
    }
    if (Application::settings().highlights.value(Settings::Modes))
        emit highlight(this, true);
}

void MessageView::msgTopicChanged(const QString& origin, const QString& topic)
{
    if (Application::settings().messages.value(Settings::Topics))
    {
        QStringList params;
        //TODO: params << senderHtml(origin) << IrcUtil::messageToHtml(topic) << d.buffer->receiver();
        receiveMessage(tr("! %1 sets topic \"%2\" on %3"), params);
    }
    if (Application::settings().highlights.value(Settings::Topics))
        emit highlight(this, true);
}

void MessageView::msgInvited(const QString& origin, const QString& receiver, const QString& channel)
{
    Q_UNUSED(receiver);
    QStringList params;
    params << senderHtml(origin) << senderHtml(channel);
    receiveMessage(tr("! %1 invited to %3"), params);
}

void MessageView::msgKicked(const QString& origin, const QString& nick, const QString& message)
{
    if (Application::settings().messages.value(Settings::Kicks))
    {
        QStringList params;
        params << senderHtml(origin) << senderHtml(nick);
        if (!message.isEmpty())
            receiveMessage(tr("! %1 kicked %2 (%3)"), params << message);
        else
            receiveMessage(tr("! %1 kicked %2"), params);
    }
    if (Application::settings().highlights.value(Settings::Kicks))
        emit highlight(this, true);

    //TODO: d.model->setStringList(Role_Names, d.buffer->names());
}

void MessageView::msgMessageReceived(const QString& origin, const QString& message)
{
    /* TODO:
    bool matches = message.contains(d.buffer->session()->nick());
    QString target = d.buffer->receiver();
    bool privmsg = !Session::isChannel(target);
    if (matches || privmsg)
        emit alert(this, true);
    else
        emit highlight(this, true);

    QStringList params;
    params << senderHtml(origin) << IrcUtil::messageToHtml(message);
    receiveMessage(tr("&lt;%1&gt; %2"), params, matches);
    */
}

void MessageView::msgNoticeReceived(const QString& origin, const QString& notice)
{
    /* TODO:
    bool matches = notice.contains(d.buffer->session()->nick());
    if (matches)
        emit alert(this, true);
    else
        emit highlight(this, true);

    QStringList params;
    params << senderHtml(origin) << IrcUtil::messageToHtml(notice);
    receiveMessage(tr("[%1] %2"), params, matches);
    */
}

void MessageView::msgCtcpRequestReceived(const QString& origin, const QString& request)
{
    /* TODO
    QStringList params;
    params << senderHtml(origin) << request.split(QRegExp("\\s")).first().toUpper();
    receiveMessage(tr("! %1 requested CTCP-%2"), params);

    if (!request.compare("VERSION", Qt::CaseInsensitive))
        d.buffer->session()->ctcpReply(origin, QString("%1 %2 %3").arg(request).arg(qApp->applicationName()).arg(qApp->applicationVersion()));
    else if (request.startsWith("PING", Qt::CaseInsensitive))
        d.buffer->session()->ctcpReply(origin, request);
    else if (request.startsWith("TIME", Qt::CaseInsensitive))
        d.buffer->session()->ctcpReply(origin, QString("%1 %2").arg(request).arg(QDateTime::currentDateTime().toString()));
    else
        qWarning("MessageView::on_ctcpRequestReceived '%s' requested '%s'", qPrintable(origin), qPrintable(request));
    */
}

void MessageView::msgCtcpReplyReceived(const QString& origin, const QString& reply)
{
    QString message = reply;
    if (message.startsWith("PING", Qt::CaseInsensitive))
    {
        QStringList params = message.split(QRegExp("\\s"), QString::SkipEmptyParts);
        if (!params.isEmpty())
        {
            QDateTime dateTime = QDateTime::fromTime_t(params.last().toInt());
            if (dateTime.isValid())
            {
                QDateTime current = QDateTime::currentDateTime();
                int msecs = dateTime.time().msecsTo(current.time());
                if (msecs < 1000)
                    message = QString("PING %1ms").arg(msecs);
                else if (msecs < 60000)
                    message = QString("PING %1s").arg(msecs / 1000.0, 0, 'f', 1);
                else
                    message = QString("PING %1m").arg(msecs / 60000);
            }
        }
    }

    QStringList params;
    params << senderHtml(origin) << message;
    receiveMessage(tr("! %1 replied CTCP-%2"), params);
}

void MessageView::msgCtcpActionReceived(const QString& origin, const QString& action)
{
    /* TODO:
    bool matches = action.contains(d.buffer->session()->nick());
    if (matches)
        emit alert(this, true);
    else
        emit highlight(this, true);

    QStringList params;
    params << senderHtml(origin) << IrcUtil::messageToHtml(action);
    receiveMessage(tr("* %1 %2"), params, matches);
    */
}

void MessageView::msgNumericMessageReceived(const QString& origin, uint code, const QStringList& params)
{
    switch (code)
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
        {
            QStringList list;
            list << senderHtml(params.value(1)) << QStringList(params.mid(2)).join(" ");
            receiveMessage("! %1 %2", list);
            return;
        }

        case Irc::RPL_WHOISUSER:
        {
            QStringList list;
            list << senderHtml(params.value(1)) << params.value(2) << params.value(3) << QStringList(params.mid(4)).join(" ");
            receiveMessage(tr("! %1 is %2@%3 %4"), list);
            return;
        }

        case Irc::RPL_WHOISSERVER:
        {
            QStringList list;
            list << senderHtml(params.value(1)) << params.value(2) << params.value(3);
            receiveMessage(tr("! %1 online via %2 (%3)"), list);
            return;
        }

        case 330: // nick user is logged in as
        {
            QStringList list;
            list << senderHtml(params.value(1)) << params.value(2) << QStringList(params.mid(3)).join(" ");
            receiveMessage(tr("! %1 %3 %2"), list);
            return;
        }

        case Irc::RPL_WHOWASUSER:
            receiveMessage(tr("! %1 was %2@%3 %4 %5"), params.mid(1));
            return;

        case Irc::RPL_ENDOFWHOWAS:
            // End of /WHOWAS list.
            return;

        case Irc::RPL_WHOISIDLE:
        {
            QString sender = senderHtml(params.value(1));

            QTime idle = QTime().addSecs(params.value(2).toInt());
            receiveMessage(tr("! %1 has been idle for %2"), QStringList() << sender << idle.toString());

            QDateTime signon = QDateTime::fromTime_t(params.value(3).toInt());
            receiveMessage(tr("! %1 has been online since %2"), QStringList() << sender << signon.toString());
            return;
        }

        case Irc::RPL_ENDOFWHOIS:
            // End of /WHOIS list.
            return;

        case Irc::RPL_WHOISCHANNELS:
        {
            QStringList list;
            list << senderHtml(params.value(1)) << params.value(2);
            receiveMessage(tr("! %1 is on channels %2"), list);
            return;
        }

        case Irc::RPL_CHANNELMODEIS:
            receiveMessage(tr("! %1 mode is %2"), QStringList() << params.value(1) << params.value(2));
            return;

        case Irc::RPL_CHANNELURL:
            receiveMessage(tr("! %1 url is %2"), QStringList() << params.value(1) << params.value(2));
            return;

        case Irc::RPL_CHANNELCREATED:
        {
            QDateTime dateTime = QDateTime::fromTime_t(params.value(2).toInt());
            receiveMessage(tr("! %1 was created %2"), QStringList() << params.value(1) << dateTime.toString());
            return;
        }

        case Irc::RPL_NOTOPIC:
            receiveMessage(tr("! %1 has no topic set"), QStringList() << params.value(1));
            return;

        case Irc::RPL_TOPIC:
            receiveMessage(tr("! %1 topic is \"%2\""), QStringList() << params.value(1) << IrcUtil::messageToHtml(params.value(2)));
            return;

        case Irc::RPL_TOPICSET:
        {
            QDateTime dateTime = QDateTime::fromTime_t(params.value(3).toInt());
            if (dateTime.isValid())
            {
                QStringList list;
                list << params.value(1) << dateTime.toString() << senderHtml(params.value(2));
                receiveMessage(tr("! %1 topic set %2 by %3"), list);
            }
            return;
        }

        case Irc::RPL_INVITING:
            receiveMessage(tr("! inviting %1 to %2"), QStringList() << params.value(1) << params.value(2));
            return;

        case Irc::RPL_VERSION:
            receiveMessage(tr("! %1 version is %2"), QStringList() << origin << params.value(1));
            return;

        case Irc::RPL_NAMREPLY:
        {
            QStringList list = params;
            list.removeAll("=");
            list.removeAll("@");
            list.removeAll("*");

            QStringList nicks = list.value(2).split(" ", QString::SkipEmptyParts);
            QStringList nickLinks;
            foreach (const QString& nick, nicks)
                nickLinks << senderHtml(nick);
            QString msg = QString("[ %2 ]").arg(nickLinks.join(" ] [ "));
            receiveMessage(tr("! %1"), QStringList(msg));

            //TODO: d.model->setStringList(Role_Names, d.buffer->names());
        }
        return;

        case Irc::RPL_ENDOFNAMES:
            return;

        case Irc::RPL_TIME:
            receiveMessage(tr("! %1 time is %2"), QStringList() << params.value(1) << params.value(2));
            return;

        default:
            break;
    }

    //qDebug() << "numeric:" << origin << code;
    receiveMessage(tr("[%1] %2"), QStringList() << QString::number(code) << params.join(" "));
}

void MessageView::msgUnknownMessageReceived(const QString& origin, const QStringList& params)
{
    // TODO
    qWarning() << "unknown:" << origin << params;
}

QString MessageView::prefixedSender(const QString& sender) const
{
    //TODO: return d.buffer->visualMode(sender) + sender;
    return QString();
}

QString MessageView::senderHtml(const QString& sender) const
{
    if (sender.isEmpty())
        return sender;

    QString prefixed = prefixedSender(sender);
    return QString("<a href='query:%1'>%2</a>").arg(sender).arg(prefixed);
}
