/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2008 Adam Higerd ahigerd@libqxt.org
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#include "mainwindow.h"
#include "messageview.h"
#include "connectdialog.h"
#include "historylineedit.h"
#include <libircclient.h>
#include <ircsession.h>
#include <irc.h>
#include <QtGui>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), alertTimer(0), connectDialog(this)
{
    centralwidget = new QWidget(this);
    gridLayout = new QGridLayout(centralwidget);
    tabWidget = new QTabWidget(centralwidget);
    gridLayout->addWidget(tabWidget, 0, 0, 1, 2);
    pushButton = new QPushButton("&Send", centralwidget);
    gridLayout->addWidget(pushButton, 1, 1, 1, 1);

    lineEdit = new HistoryLineEdit(centralwidget);
    gridLayout->addWidget(lineEdit, 1, 0, 1, 1);
    setCentralWidget(centralwidget);

    completer = new QCompleter(this);
    completer->setWidget(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    connect(completer, SIGNAL(highlighted(QString)), this, SLOT(insertCompletion(QString)));

    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(textEdited()));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(send()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(send()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));

    QShortcut* shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(partCurrentChannel()));

    shortcut = new QShortcut(QKeySequence("Alt+Left"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToPrevTab()));

    shortcut = new QShortcut(QKeySequence("Alt+Right"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToNextTab()));

    shortcut = new QShortcut(QKeySequence::MoveToPreviousPage, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToPrevPage()));

    shortcut = new QShortcut(QKeySequence::MoveToNextPage, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToNextPage()));

    shortcut = new QShortcut(Qt::Key_Tab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(autoComplete()));

    zoomInShortcut = new QShortcut(QKeySequence::ZoomIn, this);
    zoomOutShortcut = new QShortcut(QKeySequence::ZoomOut, this);

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QApplication::windowIcon());
        //trayIcon->setContextMenu(ui.menuFile);
        trayIcon->setVisible(true);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }

    QTimer::singleShot(0, this, SLOT(initialize()));

    session = new IrcSession(this);
    session->setObjectName("irc");
    QMetaObject::connectSlotsByName(this);
    session->addAutoJoinChannel("#communi");
    session->setOption(1 << 2); // LIBIRC_OPTION_STRIPNICKS

    session->setParent(0);
    thread = new QThread(this);
    session->moveToThread(thread);
    thread->start();
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("font", tabWidget->currentWidget()->font().pointSize());

    session->cmdQuit();
    thread->quit();
    thread->wait();
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        MessageView* view = qobject_cast<MessageView*>(object);
        if (view)
        {
            QApplication::sendEvent(lineEdit, event);
            lineEdit->setFocus();
            return true;
        }
    }
    return false;
}

void MainWindow::on_irc_connected()
{
    lineEdit->setFocus(Qt::OtherFocusReason);
}

void MainWindow::on_irc_disconnected()
{
    Q_ASSERT(false);
}

void MainWindow::on_irc_nickChanged(const QString& origin, const QString& nick)
{
    foreach (MessageView* view, views)
    {
        bool matches = view->matches(origin);
        bool contains = view->contains(origin);
        if (matches || contains)
        {
            view->logMessage(origin, "! %1 changed nick to %2", nick);
            if (matches)
            {
                views[nick.toLower()] = views.take(view->receiver().toLower());
                tabWidget->setTabText(tabWidget->indexOf(view), nick);
                view->setReceiver(nick);
            }
        }
    }
}

void MainWindow::on_irc_quit(const QString& origin, const QString& message)
{
    foreach (MessageView* view, views)
    {
        bool matches = view->matches(origin);
        bool contains = view->contains(origin);
        if (matches || contains)
        {
            view->logMessage(origin, "! %1 has quit (%2)", message);
            if (contains)
            {
                view->removeNick(origin);
                tabActivated(tabWidget->currentIndex());
            }
        }
    }
}

void MainWindow::on_irc_joined(const QString& origin, const QString& channel)
{
    prepareTarget(origin, channel);
    foreach (MessageView* view, views)
    {
        bool originMatches = view->matches(origin);
        bool channelMatches = view->matches(channel);
        if (originMatches || channelMatches)
        {
            view->logMessage(origin, "! %1 joined %2", channel);
            if (channelMatches)
            {
                view->addNick(origin);
                tabActivated(tabWidget->currentIndex());
            }
        }
    }
}

void MainWindow::on_irc_parted(const QString& origin, const QString& channel, const QString& message)
{
    //prepareTarget(origin, channel);
    foreach (MessageView* view, views)
    {
        bool originMatches = view->matches(origin);
        bool channelMatches = view->matches(channel);
        if (originMatches || channelMatches)
        {
            view->logMessage(origin, "! %1 parted %2", QString("%1 (%2)").arg(channel).arg(message));
            if (channelMatches)
            {
                view->removeNick(origin);
                tabActivated(tabWidget->currentIndex());
            }
        }
    }
}

void MainWindow::on_irc_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args)
{
    QString target = prepareTarget(origin, channel);
    views[target]->logMessage(origin, "! %1 %2", QString("sets mode %1 %2").arg(mode).arg(args));
}

void MainWindow::on_irc_userModeChanged(const QString& origin, const QString& mode)
{
    // TODO
    qDebug() << "user_mode:" << origin << mode;
    QString target = prepareTarget(origin, QString());
    views[target]->logMessage(origin, "! %1 %2", QString("sets mode %1").arg(mode));
    //logMessage(origin, mode, "! %1 %2", "changed the user mode");
}

void MainWindow::on_irc_topicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    QString target = prepareTarget(origin, channel);
    views[target]->logMessage(origin, "! %1 %2", QString("sets topic '%1'").arg(topic));
}

void MainWindow::on_irc_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    QString target = prepareTarget(origin, channel);
    views[target]->logMessage(origin, "! %1 %2", QString("kicked %1 (%2)").arg(nick).arg(message));
}

void MainWindow::on_irc_channelMessageReceived(const QString& origin, const QString& channel, const QString& message)
{
    QString target = prepareTarget(origin, channel);
    QString msg = message;
    if (msg.contains(connectDialog.nick()))
    {
        msg = QString("<span style='color: red'>%1</span>").arg(msg);
        alert();
    }
    views[target]->receiveMessage(origin, msg);
}

void MainWindow::on_irc_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    QString target = prepareTarget(origin, receiver);
    QString msg = message;
    if (msg.contains(connectDialog.nick()))
    {
        msg = QString("<span style='color: red'>%1</span>").arg(msg);
        alert();
    }
    views[target]->receiveMessage(origin, msg);
}

void MainWindow::on_irc_noticeReceived(const QString& origin, const QString& receiver, const QString& message)
{
    QString target = prepareTarget(origin, receiver);
    views[target]->receiveNotice(origin, message);
}

void MainWindow::on_irc_invited(const QString& origin, const QString& nick, const QString& channel)
{
    MessageView* view = static_cast<MessageView*>(tabWidget->currentWidget());
    view->logMessage(origin, "! %1 %2", QString("invited %1 to %2").arg(nick).arg(channel));
}

void MainWindow::on_irc_ctcpRequestReceived(const QString& origin, const QString& message)
{
    // TODO
    qDebug() << "ctcp_request:" << origin << message;
}

void MainWindow::on_irc_ctcpReplyReceived(const QString& origin, const QString& message)
{
    // TODO
    qDebug() << "ctcp_reply:" << origin << message;
}

void MainWindow::on_irc_ctcpActionReceived(const QString& origin, const QString& receiver, const QString& message)
{
    QString target = prepareTarget(origin, receiver);
    views[target]->receiveAction(origin, message);
}

void MainWindow::on_irc_unknownMessageReceived(const QString& origin, const QStringList& params)
{
    // TODO
    qDebug() << "unknown:" << origin << params;
    //receiveAction(origin, origin, params.join(" "));
}

void MainWindow::on_irc_numericMessageReceived(const QString& origin, uint event, const QStringList& params)
{
    MessageView* currentView = static_cast<MessageView*>(tabWidget->currentWidget());
    switch (event)
    {
    case LIBIRC_RFC_RPL_WELCOME:
        {
            views[origin.toLower()] = views.take(connectDialog.host().toLower());
            tabWidget->setTabText(0, origin);
        }
        break;

    case LIBIRC_RFC_RPL_WHOISUSER:
        {
            currentView->logMessage(params.value(1), "! %1 is %2", QString("%1@%2 (%3)").arg(params.value(2)).arg(params.value(3)).arg(params.value(5)));
        }
        return;

    case LIBIRC_RFC_RPL_WHOISSERVER:
        {
            currentView->logMessage(params.value(1), "! %1 is online via %2", QString("%1 (%2)").arg(params.value(2)).arg(params.value(3)));
        }
        return;

    case LIBIRC_RFC_RPL_WHOISOPERATOR:
        {
            qDebug() << "numeric:" << origin << event << params;
            Q_ASSERT(false);
            //currentView->logMessage(params.value(1), "! %1 is online via %2", QString("%1 (%2)").arg(params.value(2)).arg(params.value(3)));
        }
        return;

    case LIBIRC_RFC_RPL_WHOWASUSER:
        {
            qDebug() << "numeric:" << origin << event << params;
            Q_ASSERT(false);
            //currentView->logMessage(params.value(1), "! %1 is online via %2", QString("%1 (%2)").arg(params.value(2)).arg(params.value(3)));
        }
        return;

    case LIBIRC_RFC_RPL_WHOISIDLE:
        {
            QTime idle = QTime().addSecs(params.value(2).toInt());
            currentView->logMessage(params.value(1), "! %1 has been idle for %2", idle.toString());

            QDateTime signon = QDateTime::fromTime_t(params.value(3).toInt());
            currentView->logMessage(params.value(1), "! %1 has been online since %2", signon.toString());
        }
        return;

    case LIBIRC_RFC_RPL_ENDOFWHOIS:
        {
            // End of /WHOIS list.
        }
        return;

    case LIBIRC_RFC_RPL_WHOISCHANNELS:
        {
            currentView->logMessage(params.value(1), "! %1 is on channels %2", params.value(2));
        }
        return;

    case 320:
        {
            currentView->logMessage(params.value(1), "! %1 %2", params.value(2));
        }
        return;

    case LIBIRC_RFC_RPL_NOTOPIC:
        {
            QString target = prepareTarget(QString(), params.value(1));
            views[target]->logMessage(QString(), "%1! no topic set%2", QString());
        }
        return;

    case LIBIRC_RFC_RPL_TOPIC:
        {
            QString target = prepareTarget(QString(), params.value(1));
            views[target]->logMessage(QString(), "%1! topic is '%2'", params.value(2));
        }
        return;

    case 333:
        {
            QDateTime dateTime = QDateTime::fromTime_t(params.value(3).toInt());
            if (dateTime.isValid())
            {
                QString target = prepareTarget(QString(), params.value(1));
                views[target]->logMessage(params.value(2), "! topic set %2 by %1", dateTime.toString());
            }
        }
        return;

    case LIBIRC_RFC_RPL_NAMREPLY:
        {
            QStringList list = params;
            list.removeAll("=");
            list.removeAll("@");

            QString target = prepareTarget(QString(), list.value(1));
            QStringList nicks = list.value(2).split(" ", QString::SkipEmptyParts);
            views[target]->receiveNicks(nicks);
            foreach (const QString& nick, nicks)
            {
                views[target]->addNick(nick);
            }
            tabActivated(tabWidget->currentIndex());
        }
        return;

    case LIBIRC_RFC_RPL_ENDOFNAMES:
        return;

    default:
        break;
    }

    if (views.contains(origin))
        views[origin.toLower()]->logMessage(QString::number(event), "[%1] %2", params.last());
    else
        qDebug() << "numeric:" << origin << event << params;
}

void MainWindow::initialize()
{
    if (connectDialog.exec())
    {
        prepareTarget(QString(), connectDialog.host());
        session->connectToServer(connectDialog.host(), connectDialog.port(), connectDialog.nick(), "nobody", "LibIrcCLient-Qt", connectDialog.pass());
        QMetaObject::invokeMethod(session, "exec", Qt::QueuedConnection);        
    }
    else
    {
        qApp->quit();
    }
}

void MainWindow::partCurrentChannel()
{
    if (tabWidget->count() > 1)
    {
        int index = tabWidget->currentIndex();
        QString text = tabWidget->tabText(index);
        if (text.startsWith('#') || text.startsWith('&'))
            session->cmdPart(tabWidget->tabText(tabWidget->currentIndex()));
        views.remove(text);
        tabWidget->currentWidget()->deleteLater();
        tabWidget->removeTab(index);
    }
}

void MainWindow::tabActivated(int index)
{
    MessageView* view = static_cast<MessageView*>(tabWidget->currentWidget());
    completer->setModel(new QStringListModel(view->nicks(), completer));
    tabWidget->setTabIcon(index, QIcon());
    setWindowFilePath(tabWidget->tabText(tabWidget->currentIndex()));
}

void MainWindow::moveToNextTab()
{
    int index = tabWidget->currentIndex();
    if (++index >= tabWidget->count())
        index = 0;
    tabWidget->setCurrentIndex(index);
}

void MainWindow::moveToPrevTab()
{
    int index = tabWidget->currentIndex();
    if (--index < 0)
        index = tabWidget->count() - 1;
    tabWidget->setCurrentIndex(index);
}

void MainWindow::moveToNextPage()
{
    QScrollBar* scrollBar = static_cast<MessageView*>(tabWidget->currentWidget())->verticalScrollBar();
    scrollBar->triggerAction(QScrollBar::SliderPageStepAdd);
}

void MainWindow::moveToPrevPage()
{
    QScrollBar* scrollBar = static_cast<MessageView*>(tabWidget->currentWidget())->verticalScrollBar();
    scrollBar->triggerAction(QScrollBar::SliderPageStepSub);
}

void MainWindow::send()
{
    QString msg = lineEdit->text();
    if (msg.trimmed().isEmpty())
        return;

    QString receiver = tabWidget->tabText(tabWidget->currentIndex());
    MessageView* view = static_cast<MessageView*>(tabWidget->currentWidget());
    if (msg.startsWith('/'))
    {
        if (msg.startsWith("/join ") || msg.startsWith("/j "))
        {
            QString channel = msg.mid(msg.indexOf(" ") + 1);
            if (!channel.startsWith("#") && !channel.startsWith("&"))
                channel.prepend("#");
            session->cmdJoin(channel);
        }
        else if (msg.startsWith("/me "))
        {
            session->cmdMe(receiver, msg.mid(4));
            view->receiveAction(connectDialog.nick(), msg.mid(4));
        }
        else if (msg == "/w" || msg == "/who" || msg == "/names")
        {
            session->cmdNames(receiver);
        }
        else if (msg.startsWith("/w ") || msg.startsWith("/who ") || msg.startsWith("/whois "))
        {
            QString nick = msg.mid(msg.indexOf(" ") + 1);
            session->cmdWhois(nick);
        }
        else if (msg.startsWith("/part"))
        {
            partCurrentChannel();
        }
        else if (msg.startsWith("/topic"))
        {
            QString topic;
            int idx = msg.indexOf(" ");
            if (idx > 0)
                topic = msg.mid(idx + 1);
            session->cmdTopic(view->receiver(), topic);
        }
        else if (msg.startsWith("/raw "))
        {
            session->sendRaw(msg.mid(5).toUtf8());
        }
        else if (msg.startsWith("/quit"))
        {
            qApp->quit();
        }
        else
        {
            view->logMessage(QString(), "%1! unknown command %2", msg);
        }
    }
    else
    {
        session->cmdMsg(receiver, msg);
        view->receiveMessage(connectDialog.nick(), msg);
    }

    lineEdit->clear();
    lineEdit->setFocus();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::DoubleClick:
            setVisible(!isVisible());
            break;
        case QSystemTrayIcon::Trigger:
            raise();
            activateWindow();
            break;
        default:
            break;
    }
}

void MainWindow::alert()
{
    if (!alertTimer)
    {
        alertTimer = new QTimer(this);
        connect(alertTimer, SIGNAL(timeout()), this, SLOT(alert()));
    }

    if (isActiveWindow())
    {
        alertTimer->stop();
        trayIcon->setIcon(QApplication::windowIcon());
    }
    else
    {
        if (!alertTimer->isActive())
        {
            QApplication::alert(this);
            alertTimer->start(500);
        }

        if (trayIcon->icon().isNull())
            trayIcon->setIcon(QApplication::windowIcon());
        else
            trayIcon->setIcon(QIcon());
    }
}

void MainWindow::autoComplete()
{
    // store selection
    int pos = lineEdit->cursorPosition();
    int start = lineEdit->selectionStart();
    QString selected = lineEdit->selectedText();

    // select current word
    lineEdit->cursorWordForward(false);
    lineEdit->cursorWordBackward(true);
    QString word = lineEdit->selectedText();
    completer->setCompletionPrefix(word);

    // restore selection
    lineEdit->setCursorPosition(pos);
    if (start != -1)
        lineEdit->setSelection(start, selected.length());

    // complete
    if (!word.isEmpty())
        completer->complete();
}

void MainWindow::textEdited()
{
    if (completer)
        completer->setCompletionPrefix(QString());
}

void MainWindow::insertCompletion(const QString& completion)
{
    if (!completer || completer->widget() != this)
        return;

    lineEdit->cursorWordForward(false);
    lineEdit->cursorWordBackward(true);
    int pos = lineEdit->cursorPosition();
    lineEdit->insert(completion + (pos == 0 ? ": " : ""));
}

QString MainWindow::prepareTarget(const QString& sender, const QString& receiver)
{
    QString target = receiver;

    if (target == connectDialog.nick())
    {
        if (target == sender)
            target = tabWidget->tabText(0);
        else
            target = sender;
    }

    if (target.isEmpty() || target == "AUTH")
    {
        target = tabWidget->tabText(0);
    }

    if (!views.contains(target.toLower()))
    {
        MessageView* view = new MessageView(target, tabWidget);
        views.insert(target.toLower(), view);
        QSettings settings;
        if (settings.contains("font"))
        {
            QFont font = views[target]->font();
            font.setPointSize(settings.value("font").toInt());
            view->setFont(font);
        }
        connect(zoomInShortcut, SIGNAL(activated()), views[target], SLOT(zoomIn()));
        connect(zoomOutShortcut, SIGNAL(activated()), views[target], SLOT(zoomOut()));
        view->installEventFilter(this);
        tabWidget->addTab(view, target);
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
    }

    int index = tabWidget->indexOf(views[target.toLower()]);
    if (index != tabWidget->currentIndex())
        tabWidget->setTabIcon(index, QApplication::windowIcon());

    return target.toLower();
}
