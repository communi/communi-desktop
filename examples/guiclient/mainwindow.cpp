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
#include <ircsession.h>
#include <irc.h>
#include <QtGui>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), connectDialog(this)
{
    centralwidget = new QWidget(this);
    gridLayout = new QGridLayout(centralwidget);
    tabWidget = new QTabWidget(centralwidget);
    gridLayout->addWidget(tabWidget, 0, 0, 1, 2);
    pushButton = new QPushButton("&Send", centralwidget);
    gridLayout->addWidget(pushButton, 1, 1, 1, 1);

    lineEdit = new QLineEdit(centralwidget);
    gridLayout->addWidget(lineEdit, 1, 0, 1, 1);
    setCentralWidget(centralwidget);

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

    setWindowTitle("LibIrcCLient-Qt demo");
    QTimer::singleShot(0, this, SLOT(initialize()));

    session = new IrcSession(this);
    session->setObjectName("irc");
    QMetaObject::connectSlotsByName(this);
    session->addAutoJoinChannel("#communi");
    session->setOption(1 << 2); // LIBIRC_OPTION_STRIPNICKS

    session->setParent(0);
    QThread* thread = new QThread(this);
    session->moveToThread(thread);
    thread->start();
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        MessageView* view = qobject_cast<MessageView*>(object);
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (view && keyEvent->modifiers() == Qt::NoModifier)
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
                views[nick] = views.take(view->receiver());
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
                view->removeNick(origin);
        }
    }
}

void MainWindow::on_irc_joined(const QString& origin, const QString& channel)
{
    qDebug() << "join:" << origin << channel;
    prepareTarget(origin, channel);
    foreach (MessageView* view, views)
    {
        bool originMatches = view->matches(origin);
        bool channelMatches = view->matches(channel);
        if (originMatches || channelMatches)
        {
            view->logMessage(origin, "! %1 joined %2", channel);
            if (channelMatches)
                view->addNick(origin);
        }
    }
}

void MainWindow::on_irc_parted(const QString& origin, const QString& channel, const QString& message)
{
    prepareTarget(origin, channel);
    foreach (MessageView* view, views)
    {
        bool originMatches = view->matches(origin);
        bool channelMatches = view->matches(channel);
        if (originMatches || channelMatches)
        {
            view->logMessage(origin, "! %1 parted %2", channel);
            if (channelMatches)
                view->removeNick(origin);
        }
    }
}

// TODO: check these:
void MainWindow::on_irc_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args)
{
    qDebug() << "channel_mode:" << origin << channel << mode << args;
    QString target = prepareTarget(origin, channel);
    views[target]->logMessage(origin, "! %1 %2", QString("sets mode %1 %2").arg(mode).arg(args));
    //logMessage(origin, channel, "! %1 %2", "changed the channel mode");
}

void MainWindow::on_irc_userModeChanged(const QString& origin, const QString& mode)
{
    qDebug() << "user_mode:" << origin << mode;
    QString target = prepareTarget(origin, QString());
    views[target]->logMessage(origin, "! %1 %2", QString("sets mode %1").arg(mode));
    //logMessage(origin, mode, "! %1 %2", "changed the user mode");
}

void MainWindow::on_irc_topicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    qDebug() << "topic:" << origin << channel << topic;
    QString target = prepareTarget(origin, channel);
    views[target]->logMessage(origin, "! %1 %2", QString("sets topic %1").arg(topic));
    //logMessage(origin, channel, "! %1 %2", "changed the channel topic");
}

void MainWindow::on_irc_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    qDebug() << "kick:" << origin << channel << nick << message;
    //logMessage(origin, channel, "! %1 %2", "kicked from channel");
}

void MainWindow::on_irc_channelMessageReceived(const QString& origin, const QString& channel, const QString& message)
{
    QString target = prepareTarget(origin, channel);
    views[target]->receiveMessage(origin, message);
}

void MainWindow::on_irc_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    QString target = prepareTarget(origin, receiver);
    views[target]->receiveMessage(origin, message);
}

void MainWindow::on_irc_noticeReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "notice:" << origin << receiver << message;
    QString target = prepareTarget(origin, receiver);
    views[target]->receiveNotice(origin, message);
}

void MainWindow::on_irc_invited(const QString& origin, const QString& nick, const QString& channel)
{
    qDebug() << "invite:" << origin << nick << channel;
}

void MainWindow::on_irc_ctcpRequestReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_request:" << origin << message;
}

void MainWindow::on_irc_ctcpReplyReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_reply:" << origin << message;
}

void MainWindow::on_irc_ctcpActionReceived(const QString& origin, const QString& message)
{
    qDebug() << "ctcp_action:" << origin << message;
    //receiveAction(origin, origin, message);
}

void MainWindow::on_irc_unknownMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "unknown:" << origin << params;
    //receiveAction(origin, origin, params.join(" "));
}

void MainWindow::on_irc_numericMessageReceived(const QString& origin, uint event, const QStringList& params)
{
    switch (event)
    {
    case 1:
        {
            views[origin] = views.take(connectDialog.host());
            tabWidget->setTabText(0, origin);
        }
        break;

    case 332:
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

    case 353:
        {
            QStringList list = params;
            list.removeAll("=");
            list.removeAll("@");

            QString target = prepareTarget(QString(), list.value(1));
            foreach (const QString& nick, list.value(2).split(" "))
            {
                views[target]->addNick(nick);
            }
        }
        return;

    case 366:
        return;

    default:
        break;
    }

    if (views.contains(origin))
        views[origin]->logMessage(QString::number(event), "[%1] %2", params.join(" "));
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
        //receiveMessage(connectDialog.host(), connectDialog.host(),
            //QString("Connecting to %1:%2...").arg(connectDialog.host()).arg(connectDialog.port()));
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
    tabWidget->setTabIcon(index, QIcon());
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
    QScrollBar* scrollBar = qobject_cast<MessageView*>(tabWidget->currentWidget())->verticalScrollBar();
    scrollBar->triggerAction(QScrollBar::SliderPageStepAdd);
}

void MainWindow::moveToPrevPage()
{
    QScrollBar* scrollBar = qobject_cast<MessageView*>(tabWidget->currentWidget())->verticalScrollBar();
    scrollBar->triggerAction(QScrollBar::SliderPageStepSub);
}

void MainWindow::send()
{
    QString msg = lineEdit->text();
    if (msg.trimmed().isEmpty())
        return;

    if (msg.startsWith('/'))
    {
        if (msg.startsWith("/join "))
        {
            if (msg.mid(6,1) == "#" || msg.mid(6,1) == "&")
                session->cmdJoin(msg.mid(6));

            //receiveMessage(tabWidget->tabText(0), msg.mid(6), QString("you have joined %1").arg(msg.mid(6)));
            tabWidget->setCurrentWidget(views.value(msg.mid(6)));
        }
        else if (msg.startsWith("/me "))
        {
            //emit sendMessage(IRCName(condiag.nickname().toUtf8()),
            //        tabWidget->tabText(tabWidget->currentIndex()).toUtf8(),
            //        QByteArray("\001ACTION " + msg.mid(4) + "\001"));
        }
        else if (msg.startsWith("/names"))
        {
            //if (msg == "/names")
            //    irc.call("NAMES", QVariant(), tabWidget->tabText(tabWidget->currentIndex()).toUtf8());
            //else
            //    irc.call("NAMES", QVariant(), msg.mid(7));
        }
        else if (msg == "/part")
        {
            partCurrentChannel();
        }
        else if (msg.startsWith("/raw "))
        {
            //irc.call("raw", msg.mid(5));
        }
        else if (msg == "/quit")
        {
            qApp->quit();
        }
        else
        {
            //receiveMessage(tabWidget->tabText(0), tabWidget->tabText(0), QString("unknown command %1"). arg(msg));
        }
    }
    else
    {
        //emit sendMessage(IRCName(condiag.nickname().toUtf8()),
                //tabWidget->tabText(tabWidget->currentIndex()).toUtf8(), msg);
        QString receiver = tabWidget->tabText(tabWidget->currentIndex());
        session->cmdMsg(receiver, msg);
        //receiveMessage(connectDialog.nick(), receiver, msg);
    }

    lineEdit->clear();
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

    if (!views.contains(target))
    {
        views[target] = new MessageView(target, tabWidget);
        views[target]->setFrameShape(QFrame::NoFrame);
        views[target]->installEventFilter(this);
        tabWidget->addTab(views[target], target);
    }

    int index = tabWidget->indexOf(views[target]);
    if (index != tabWidget->currentIndex())
        tabWidget->setTabIcon(index, qApp->style()->standardIcon(QStyle::SP_DialogNoButton));

    return target;
}
