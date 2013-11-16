/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "appwindow.h"
#include "connectpage.h"
#include "chatpage.h"
#include <IrcConnection>
#include <QApplication>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <IrcBuffer>
#include <QShortcut>
#include <QTimer>

AppWindow::AppWindow(QWidget* parent) : MainWindow(parent)
{
    d.editedConnection = 0;

    d.normalIcon.addFile(":/icons/16x16/communi.png");
    d.normalIcon.addFile(":/icons/24x24/communi.png");
    d.normalIcon.addFile(":/icons/32x32/communi.png");
    d.normalIcon.addFile(":/icons/48x48/communi.png");
    d.normalIcon.addFile(":/icons/64x64/communi.png");
    d.normalIcon.addFile(":/icons/128x128/communi.png");

    d.alertIcon.addFile(":/icons/16x16/communi-alert.png");
    d.alertIcon.addFile(":/icons/24x24/communi-alert.png");
    d.alertIcon.addFile(":/icons/32x32/communi-alert.png");
    d.alertIcon.addFile(":/icons/48x48/communi-alert.png");
    d.alertIcon.addFile(":/icons/64x64/communi-alert.png");
    d.alertIcon.addFile(":/icons/128x128/communi-alert.png");

#ifndef Q_OS_MAC
    setWindowIcon(d.normalIcon);
    qApp->setWindowIcon(d.normalIcon);
#endif // Q_OS_MAC

    d.stack = new QStackedWidget(this);
    connect(d.stack, SIGNAL(currentChanged(int)), this, SLOT(updateTitle()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(d.stack);
    layout->setMargin(0);

    d.connectPage = new ConnectPage(this);
    connect(d.connectPage, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(d.connectPage, SIGNAL(rejected()), this, SLOT(onRejected()));

    d.chatPage = new ChatPage(this);
    connect(d.chatPage, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(updateTitle()));

    d.stack->addWidget(d.connectPage);
    d.stack->addWidget(d.chatPage);

    QShortcut* shortcut = new QShortcut(QKeySequence::Quit, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(doConnect()));

    shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), d.chatPage, SLOT(closeBuffer()));

    d.chatPage->init();
    if (d.chatPage->connections().isEmpty())
        doConnect();
}

AppWindow::~AppWindow()
{
}

QList<IrcConnection*> AppWindow::connections() const
{
    return d.chatPage->connections();
}

void AppWindow::addConnection(IrcConnection* connection)
{
    d.chatPage->addConnection(connection);
    d.stack->setCurrentWidget(d.chatPage);
}

void AppWindow::removeConnection(IrcConnection* connection)
{
    d.chatPage->removeConnection(connection);
    if (d.chatPage->connections().isEmpty())
        doConnect();
}

QSize AppWindow::sizeHint() const
{
    return QSize(800, 600);
}

void AppWindow::closeEvent(QCloseEvent* event)
{
    if (isVisible()) {
        d.chatPage->uninit();
        QList<IrcConnection*> connections = d.chatPage->connections();
        foreach (IrcConnection* connection, connections) {
            QString reason = tr("%1 %2 - http://%3").arg(QCoreApplication::applicationName())
                                                    .arg(QCoreApplication::applicationVersion())
                                                    .arg(QCoreApplication::organizationDomain());
            connection->quit(reason);
            connection->close();
        }

        // let the sessions close in the background
        hide();
        event->ignore();
        QTimer::singleShot(1000, qApp, SLOT(quit()));
    }
}

void AppWindow::doConnect()
{
    d.connectPage->buttonBox()->button(QDialogButtonBox::Cancel)->setEnabled(!d.chatPage->connections().isEmpty());
    d.stack->setCurrentWidget(d.connectPage);
}

void AppWindow::onAccepted()
{
    IrcConnection* connection = d.editedConnection;
    if (!connection)
        connection = new IrcConnection(this);
    connection->setHost(d.connectPage->host());
    connection->setPort(d.connectPage->port());
    connection->setSecure(d.connectPage->isSecure());
    connection->setNickName(d.connectPage->nickName());
    connection->setRealName(d.connectPage->realName());
    connection->setUserName(d.connectPage->userName());
    connection->setDisplayName(d.connectPage->displayName());
    connection->setPassword(d.connectPage->password());
    if (!d.editedConnection)
        d.chatPage->addConnection(connection);
    d.editedConnection = 0;
    d.stack->setCurrentWidget(d.chatPage);
}

void AppWindow::onRejected()
{
    d.editedConnection = 0;
    d.stack->setCurrentWidget(d.chatPage);
}

void AppWindow::updateTitle()
{
    IrcBuffer* buffer = d.chatPage->currentBuffer();
    if (!buffer || d.stack->currentWidget() == d.connectPage)
        setWindowTitle(QCoreApplication::applicationName());
    else
        setWindowTitle(tr("%1 - %2").arg(buffer->title(), QCoreApplication::applicationName()));
}

void AppWindow::editConnection(IrcConnection* connection)
{
    d.connectPage->setHost(connection->host());
    d.connectPage->setPort(connection->port());
    d.connectPage->setSecure(connection->isSecure());
    d.connectPage->setNickName(connection->nickName());
    d.connectPage->setRealName(connection->realName());
    d.connectPage->setUserName(connection->userName());
    d.connectPage->setDisplayName(connection->displayName());
    d.connectPage->setPassword(connection->password());
    d.stack->setCurrentWidget(d.connectPage);
    d.editedConnection = connection;
    doConnect();
}
