/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "mainwindow.h"
#include "settingspage.h"
#include "textdocument.h"
#include "connectpage.h"
#include "bufferview.h"
#include "helppopup.h"
#include "chatpage.h"
#include "dock.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <QApplication>
#include <QCloseEvent>
#include <QPushButton>
#include <IrcBuffer>
#include <QShortcut>
#include <QSettings>
#include <QMenuBar>
#include <QTimer>
#include <QUuid>
#include <QMenu>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    d.view = 0;

    // TODO
    QDir::addSearchPath("black", ":/images/black");
    QDir::addSearchPath("gray", ":/images/gray");
    QDir::addSearchPath("white", ":/images/white");

#ifndef Q_OS_MAC
    QIcon icon;
    icon.addFile(":/communi-16.png");
    icon.addFile(":/communi-24.png");
    icon.addFile(":/communi-32.png");
    icon.addFile(":/communi-48.png");
    icon.addFile(":/communi-64.png");
    icon.addFile(":/communi-128.png");
    icon.addFile(":/communi-256.png");
    icon.addFile(":/communi-512.png");
    setWindowIcon(icon);
    qApp->setWindowIcon(icon);
#endif // Q_OS_MAC

    d.stack = new QStackedWidget(this);
    connect(d.stack, SIGNAL(currentChanged(int)), this, SLOT(updateTitle()));

    setCentralWidget(d.stack);

    d.chatPage = new ChatPage(this);
    setCurrentView(d.chatPage->currentView());
    connect(d.chatPage, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(updateTitle()));
    connect(d.chatPage, SIGNAL(currentViewChanged(BufferView*)), this, SLOT(setCurrentView(BufferView*)));
    connect(this, SIGNAL(connectionAdded(IrcConnection*)), d.chatPage, SLOT(addConnection(IrcConnection*)));
    connect(this, SIGNAL(connectionRemoved(IrcConnection*)), d.chatPage, SLOT(removeConnection(IrcConnection*)));
    connect(this, SIGNAL(connectionRemoved(IrcConnection*)), this, SLOT(removeConnection(IrcConnection*)));

    d.stack->addWidget(d.chatPage);

    QShortcut* shortcut = new QShortcut(QKeySequence::Quit, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(doConnect()));

    shortcut = new QShortcut(QKeySequence::Preferences, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(showSettings()));

    shortcut = new QShortcut(QKeySequence::Close, d.chatPage);
    connect(shortcut, SIGNAL(activated()), d.chatPage, SLOT(closeBuffer()));

    d.dock = new Dock(this);
    connect(d.chatPage, SIGNAL(messageHighlighted(IrcMessage*)), d.dock, SLOT(alert(IrcMessage*)));
    connect(d.chatPage, SIGNAL(privateMessageReceived(IrcMessage*)), d.dock, SLOT(alert(IrcMessage*)));

#ifdef Q_OS_MAC
    QMenu* menu = new QMenu(this);
    menuBar()->addMenu(menu);

    QAction* action = new QAction(tr("Preferences"), this);
    action->setMenuRole(QAction::PreferencesRole);
    connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
    menu->addAction(action);
#endif // Q_OS_MAC

    restoreState();

    if (d.connections.isEmpty())
        doConnect();
}

MainWindow::~MainWindow()
{
}

void MainWindow::saveState()
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("settings", d.chatPage->saveSettings());
    settings.setValue("state", d.chatPage->saveState());

    QVariantList states;
    foreach (IrcConnection* connection, d.connections) {
        QVariantMap state;
        state.insert("connection", connection->saveState());
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
        if (model)
            state.insert("model", model->saveState());
        states += state;
    }
    settings.setValue("connections", states);
}

void MainWindow::restoreState()
{
    QSettings settings;
    if (settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());

    d.chatPage->restoreSettings(settings.value("settings").toByteArray());

    foreach (const QVariant& v, settings.value("connections").toList()) {
        QVariantMap state = v.toMap();
        IrcConnection* connection = new IrcConnection(d.chatPage);
        connection->restoreState(state.value("connection").toByteArray());
        addConnection(connection);
        if (state.contains("model") && connection->isEnabled()) {
            connection->setProperty("__modelState__", state.value("model").toByteArray());
            connect(connection, SIGNAL(connected()), this, SLOT(delayedRestoreConnection()));
        }
    }

    d.chatPage->restoreState(settings.value("state").toByteArray());
}

BufferView* MainWindow::currentView() const
{
    return d.view;
}

void MainWindow::setCurrentView(BufferView* view)
{
    if (d.view != view) {
        d.view = view;
        emit currentViewChanged(view);
    }
}

QList<IrcConnection*> MainWindow::connections() const
{
    return d.connections;
}

void MainWindow::addConnection(IrcConnection* connection)
{
    QVariantMap ud = connection->userData();
    if (!ud.contains("uuid")) {
        ud.insert("uuid", QUuid::createUuid());
        connection->setUserData(ud);
    }

    d.connections += connection;
    connect(connection, SIGNAL(destroyed(IrcConnection*)), this, SLOT(removeConnection(IrcConnection*)));
    emit connectionAdded(connection);
}

void MainWindow::removeConnection(IrcConnection* connection)
{
    if (d.connections.removeOne(connection))
        emit connectionRemoved(connection);
    if (d.connections.isEmpty())
        doConnect();
}

void MainWindow::push(QWidget* page)
{
    if (d.stack->currentWidget())
        d.stack->currentWidget()->setEnabled(false);
    d.stack->addWidget(page);
    d.stack->setCurrentWidget(page);
}

void MainWindow::pop()
{
    QWidget* page = d.stack->currentWidget();
    if (!qobject_cast<ChatPage*>(page)) {
        d.stack->removeWidget(page);
        d.stack->setCurrentIndex(d.stack->count() - 1);
        if (d.stack->currentWidget())
            d.stack->currentWidget()->setEnabled(true);
        page->deleteLater();
    }
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

bool MainWindow::event(QEvent* event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow())
            emit activated();
    }
    return QMainWindow::event(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (isVisible()) {
        saveState();

        foreach (IrcConnection* connection, d.connections) {
            connection->quit(qApp->property("description").toString());
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
    for (int i = 0; i < d.stack->count(); ++i) {
        ConnectPage* page = qobject_cast<ConnectPage*>(d.stack->widget(i));
        if (page) {
            d.stack->setCurrentWidget(page);
            return;
        }
    }

    ConnectPage* page = new ConnectPage(this);
    page->buttonBox()->button(QDialogButtonBox::Cancel)->setEnabled(!d.connections.isEmpty());
    connect(page, SIGNAL(accepted()), this, SLOT(onConnectAccepted()));
    connect(page, SIGNAL(rejected()), this, SLOT(pop()));
    push(page);
}

void MainWindow::onEditAccepted()
{
    ConnectPage* page = qobject_cast<ConnectPage*>(sender());
    if (page) {
        IrcConnection* connection = page->connection();
        connection->setHost(page->host());
        connection->setPort(page->port());
        connection->setSecure(page->isSecure());
        connection->setNickName(page->nickName());
        connection->setRealName(page->realName());
        connection->setUserName(page->userName());
        connection->setDisplayName(page->displayName());
        connection->setPassword(page->password());
        connection->setSaslMechanism(page->saslMechanism());
        pop();
    }
}

void MainWindow::onConnectAccepted()
{
    ConnectPage* page = qobject_cast<ConnectPage*>(sender());
    if (page) {
        IrcConnection* connection = new IrcConnection(d.chatPage);
        connection->setHost(page->host());
        connection->setPort(page->port());
        connection->setSecure(page->isSecure());
        connection->setNickName(page->nickName());
        connection->setRealName(page->realName());
        connection->setUserName(page->userName());
        connection->setDisplayName(page->displayName());
        connection->setPassword(page->password());
        connection->setSaslMechanism(page->saslMechanism());
        addConnection(connection);
        pop();
    }
}

void MainWindow::onSettingsAccepted()
{
    SettingsPage* page = qobject_cast<SettingsPage*>(sender());
    if (page) {
        d.chatPage->setTheme(page->theme());
        pop();
    }
}

void MainWindow::updateTitle()
{
    IrcBuffer* buffer = d.chatPage->currentBuffer();
    if (!buffer || d.stack->currentWidget() != d.chatPage)
        setWindowTitle(QCoreApplication::applicationName());
    else
        setWindowTitle(tr("%1 - %2").arg(buffer->title(), QCoreApplication::applicationName()));
}

void MainWindow::showSettings()
{
    for (int i = 0; i < d.stack->count(); ++i) {
        SettingsPage* page = qobject_cast<SettingsPage*>(d.stack->widget(i));
        if (page) {
            d.stack->setCurrentWidget(page);
            return;
        }
    }

    SettingsPage* page = new SettingsPage(d.stack);
    page->setTheme(d.chatPage->theme());

    connect(page, SIGNAL(accepted()), this, SLOT(onSettingsAccepted()));
    connect(page, SIGNAL(rejected()), this, SLOT(pop()));
    push(page);
}

void MainWindow::showHelp()
{
    HelpPopup* help = new HelpPopup(this);
    help->popup();
}

void MainWindow::editConnection(IrcConnection* connection)
{
    ConnectPage* page = new ConnectPage(connection, this);
    connect(page, SIGNAL(accepted()), this, SLOT(onEditAccepted()));
    connect(page, SIGNAL(rejected()), this, SLOT(pop()));
    page->setHost(connection->host());
    page->setPort(connection->port());
    page->setSecure(connection->isSecure());
    page->setNickName(connection->nickName());
    page->setRealName(connection->realName());
    page->setUserName(connection->userName());
    page->setDisplayName(connection->displayName());
    page->setPassword(connection->password());
    push(page);
}

void MainWindow::restoreConnection(IrcConnection* connection)
{
    if (!connection && !d.restoredConnections.isEmpty())
        connection = d.restoredConnections.dequeue();
    if (connection && connection->isConnected()) {
        QByteArray state = connection->property("__modelState__").toByteArray();
        if (!state.isNull()) {
            IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
            if (model && model->count() == 1)
                model->restoreState(state);
        }
        connection->setProperty("__modelState__", QVariant());
    }
}

void MainWindow::delayedRestoreConnection()
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection) {
        // give bouncers 1 second to start joining channels, otherwise a
        // non-bouncer connection is assumed and model state is restored
        disconnect(connection, SIGNAL(connected()), this, SLOT(delayedRestoreConnection()));
        QTimer::singleShot(1000, this, SLOT(restoreConnection()));
        d.restoredConnections.enqueue(connection);
    }
}
