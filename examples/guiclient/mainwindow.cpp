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
#include "connectdialog.h"
#include <ircsession.h>
#include <irc.h>

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
    connect(shortcut, SIGNAL(activated()), this, SLOT(moveToPreviousPage()));

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
        QTextBrowser* browser = qobject_cast<QTextBrowser*>(object);
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (browser && keyEvent->modifiers() == Qt::NoModifier)
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
    qDebug() << "nick:" << origin << nick;
    logMessage(origin, origin, "! %1 %2", nick);
}

void MainWindow::on_irc_quit(const QString& origin, const QString& message)
{
    qDebug() << "quit:" << origin << message;
    if (channels.contains(origin))
        logMessage(origin, origin, "! %1 Quit: %2", message);
    logMessage(origin, "", "! %1 Quit: %2", message);
}

void MainWindow::on_irc_joined(const QString& origin, const QString& channel)
{
    qDebug() << "join:" << origin << channel;
    if (channels.contains(origin))
        logMessage(origin, origin, "! %1 joined %2", channel);
    logMessage(origin, channel, "! %1 %2", QString("joined %1").arg(channel));
}

void MainWindow::on_irc_parted(const QString& origin, const QString& channel, const QString& message)
{
    qDebug() << "part:" << origin << channel << message;
    if (origin != connectDialog.nick())
        logMessage(origin, channel, "! %1 %2", QString("left %1").arg(channel));
}

void MainWindow::on_irc_channelModeChanged(const QString& origin, const QString& channel, const QString& mode, const QString& args)
{
    qDebug() << "channel_mode:" << origin << channel << mode << args;
    logMessage(origin, channel, "! %1 %2", "changed the channel mode");
}

void MainWindow::on_irc_userModeChanged(const QString& origin, const QString& mode)
{
    qDebug() << "user_mode:" << origin << mode;
    logMessage(origin, mode, "! %1 %2", "changed the user mode");
}

void MainWindow::on_irc_topicChanged(const QString& origin, const QString& channel, const QString& topic)
{
    qDebug() << "topic:" << origin << channel << topic;
    logMessage(origin, channel, "! %1 %2", "changed the channel topic");
}

void MainWindow::on_irc_kicked(const QString& origin, const QString& channel, const QString& nick, const QString& message)
{
    qDebug() << "kick:" << origin << channel << nick << message;
    logMessage(origin, channel, "! %1 %2", "kicked from channel");
}

void MainWindow::on_irc_channelMessageReceived(const QString& origin, const QString& channel, const QString& message)
{
    receiveMessage(origin, channel, message);
}

void MainWindow::on_irc_privateMessageReceived(const QString& origin, const QString& receiver, const QString& message)
{
    receiveMessage(origin, receiver, message);
}

void MainWindow::on_irc_noticeReceived(const QString& origin, const QString& receiver, const QString& message)
{
    qDebug() << "notice:" << origin << receiver << message;
    receiveNotice(origin, receiver, message);
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
    receiveAction(origin, origin, message);
}

void MainWindow::on_irc_unknownMessageReceived(const QString& origin, const QStringList& params)
{
    qDebug() << "unknown:" << origin << params;
    receiveAction(origin, origin, params.join(" "));
}

void MainWindow::on_irc_numericMessageReceived(const QString& origin, uint event, const QStringList& params)
{
    switch (event)
    {
    case 1:
        channels[origin] = channels.take(connectDialog.host());
        tabWidget->setTabText(0, origin);
        break;
    default:
        break;
    }

    qDebug() << "numeric:" << origin << event << params;
    receiveMessage(origin, origin, params.join(" "));
}

void MainWindow::initialize()
{
    if (connectDialog.exec())
    {
        session->connectToServer(connectDialog.host(), connectDialog.port(), connectDialog.nick(), "nobody", "LibIrcCLient-Qt", connectDialog.pass());
        QMetaObject::invokeMethod(session, "exec", Qt::QueuedConnection);
        receiveMessage(connectDialog.host(), connectDialog.host(),
            QString("Connecting to %1:%2...").arg(connectDialog.host()).arg(connectDialog.port()));
    }
    else
    {
        qApp->quit();
    }
}

void MainWindow::receiveMessage(const QString& sender, const QString& receiver, const QString& message)
{
    logMessage(sender, receiver, "<%1> %2", message);
}

void MainWindow::receiveNotice(const QString& sender, const QString& receiver, const QString& message)
{
    logMessage(sender, receiver, "[%1] %2", message);
}

void MainWindow::receiveAction(const QString& sender, const QString& receiver, const QString& message)
{
    logMessage(sender, receiver, "* %1 %2", message);
}

void MainWindow::logMessage(const QString& sender, const QString& receiver, const QString& format, const QString& message)
{
    QString source = sender; //Irc::nickFromTarget(sender);
    QString target = receiver; //Irc::nickFromTarget(receiver);

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

    if (!channels.contains(target))
    {
        channels[target] = new QTextBrowser(tabWidget);
        channels[target]->setFrameShape(QFrame::NoFrame);
        channels[target]->installEventFilter(this);
        tabWidget->addTab(channels[target], target);
    }

    channels[target]->append(format.arg(source).arg(Irc::colorStripFromMirc(message)));
    int index = tabWidget->indexOf(channels[target]);
    if (index != tabWidget->currentIndex())
        tabWidget->setTabIcon(index, qApp->style()->standardIcon(QStyle::SP_DialogNoButton));
}

void MainWindow::partCurrentChannel()
{
    if (tabWidget->count() > 1)
    {
        int index = tabWidget->currentIndex();
        QString text = tabWidget->tabText(index);
        if (text.startsWith('#') || text.startsWith('&'))
            session->cmdPart(tabWidget->tabText(tabWidget->currentIndex()));
        channels.remove(text);
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
    QScrollBar* scrollBar = qobject_cast<QTextBrowser*>(tabWidget->currentWidget())->verticalScrollBar();
    scrollBar->triggerAction(QScrollBar::SliderPageStepAdd);
}

void MainWindow::moveToPrevPage()
{
    QScrollBar* scrollBar = qobject_cast<QTextBrowser*>(tabWidget->currentWidget())->verticalScrollBar();
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

            receiveMessage(tabWidget->tabText(0), msg.mid(6), QString("you have joined %1").arg(msg.mid(6)));
            tabWidget->setCurrentWidget(channels.value(msg.mid(6)));
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
            receiveMessage(tabWidget->tabText(0), tabWidget->tabText(0), QString("unknown command %1"). arg(msg));
        }
    }
    else
    {
        //emit sendMessage(IRCName(condiag.nickname().toUtf8()),
                //tabWidget->tabText(tabWidget->currentIndex()).toUtf8(), msg);
        QString receiver = tabWidget->tabText(tabWidget->currentIndex());
        session->cmdMsg(receiver, msg);
        receiveMessage(connectDialog.nick(), receiver, msg);
    }

    lineEdit->clear();
}
