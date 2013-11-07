/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "mainwindow.h"
#include "connectpage.h"
#include "sharedtimer.h"
#include "chatpage.h"
#include <QDesktopServices>
#include <IrcBufferModel>
#include <IrcConnection>
#include <QApplication>
#include <QCloseEvent>
#include <QPushButton>
#include <IrcChannel>
#include <QSettings>
#include <IrcBuffer>
#include <QShortcut>
#include <QTimer>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QStackedWidget(parent)
{
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

    QSettings settings;
    settings.beginGroup("Geometries");
    if (settings.contains("window"))
        restoreGeometry(settings.value("window").toByteArray());

    d.connectPage = new ConnectPage(this);
    connect(d.connectPage, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(d.connectPage, SIGNAL(rejected()), this, SLOT(onRejected()));
    addWidget(d.connectPage);

    d.chatPage = new ChatPage(this);
    connect(d.chatPage, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(setCurrentBuffer(IrcBuffer*)));
    addWidget(d.chatPage);

    setCurrentBuffer(0);

    QShortcut* shortcut = new QShortcut(QKeySequence::Quit, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(doConnect()));

    shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeBuffer()));

    restoreConnections();
}

MainWindow::~MainWindow()
{
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (isVisible()) {
        QSettings settings;
        settings.beginGroup("Geometries");
        settings.setValue("window", saveGeometry());

        saveConnections();
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

void MainWindow::doConnect()
{
    d.connectPage->buttonBox()->button(QDialogButtonBox::Cancel)->setEnabled(!d.chatPage->connections().isEmpty());
    setCurrentWidget(d.connectPage);
}

void MainWindow::onAccepted()
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
    setCurrentWidget(d.chatPage);
}

void MainWindow::onRejected()
{
    d.editedConnection = 0;
    setCurrentWidget(d.chatPage);
}

void MainWindow::closeBuffer(IrcBuffer* buffer)
{
    if (!buffer)
        buffer = d.chatPage->currentBuffer();

    IrcChannel* channel = buffer->toChannel();
    if (channel && channel->isActive()) {
        QString reason = tr("%1 %2 - http://%3").arg(QCoreApplication::applicationName())
                                                .arg(QCoreApplication::applicationVersion())
                                                .arg(QCoreApplication::organizationDomain());
        channel->part(reason);
    }
    buffer->deleteLater();
}

void MainWindow::setCurrentBuffer(IrcBuffer* buffer)
{
    if (buffer) {
        setCurrentWidget(d.chatPage);
        setWindowTitle(tr("%2 - Communi %1").arg(IRC_VERSION_STR).arg(buffer->title()));
    } else {
        doConnect();
        setWindowTitle(tr("Communi %1").arg(IRC_VERSION_STR));
    }
}

void MainWindow::editConnection(IrcConnection* connection)
{
    d.connectPage->setHost(connection->host());
    d.connectPage->setPort(connection->port());
    d.connectPage->setSecure(connection->isSecure());
    d.connectPage->setNickName(connection->nickName());
    d.connectPage->setRealName(connection->realName());
    d.connectPage->setUserName(connection->userName());
    d.connectPage->setDisplayName(connection->displayName());
    d.connectPage->setPassword(connection->password());
    setCurrentWidget(d.connectPage);
    d.editedConnection = connection;
    doConnect();
}

void MainWindow::restoreConnections()
{
    QSettings settings;
    settings.beginGroup("Connections");
    foreach (const QVariant& state, settings.value("connections").toList()) {
        IrcConnection* connection = new IrcConnection(this);
        connection->restoreState(state.toByteArray());
        d.chatPage->addConnection(connection);
    }
    if (d.chatPage->connections().isEmpty())
        doConnect();
}

void MainWindow::saveConnections()
{
    QSettings settings;
    settings.beginGroup("Connections");
    QVariantList states;
    foreach (IrcConnection* connection, d.chatPage->connections())
        states += connection->saveState();
    settings.setValue("connections", states);
}
