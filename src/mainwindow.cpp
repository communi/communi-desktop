/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "mainwindow.h"
#include "application.h"
#include "connectionwizard.h"
#include "sessionstackview.h"
#include "sessiontreewidget.h"
#include "messagestackview.h"
#include "sessiontreeitem.h"
#include "connectioninfo.h"
#include "addviewdialog.h"
#include "systemnotifier.h"
#include "searchpopup.h"
#include "messageview.h"
#include "homepage.h"
#include "overlay.h"
#include "toolbar.h"
#include "session.h"
#include "qtdocktile.h"
#include <QCloseEvent>
#include <QSettings>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    treeWidget(0), trayIcon(0), dockTile(0)
{
    stackView = new SessionStackView(this);

    HomePage* homePage = new HomePage(stackView);
    connect(homePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
    stackView->insertWidget(0, homePage);

    QSplitter* splitter = new QSplitter(this);
    splitter->setHandleWidth(1);
    splitter->addWidget(stackView);
    setCentralWidget(splitter);

    createTree();

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        trayIcon = new TrayIcon(this);
        trayIcon->setIcon(QApplication::windowIcon());
        trayIcon->setVisible(true);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }

    if (QtDockTile::isAvailable())
        dockTile = new QtDockTile(this);

    QShortcut* shortcut = new QShortcut(QKeySequence::Quit, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(connectTo()), Qt::QueuedConnection);

    shortcut = new QShortcut(QKeySequence::AddTab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(addView()), Qt::QueuedConnection);

    shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeView()));

    searchShortcut = new QShortcut(this);
    connect(searchShortcut, SIGNAL(activated()), this, SLOT(searchView()));

#ifdef Q_WS_MAC
    QMenu* menu = new QMenu(this);
    menuBar()->addMenu(menu);

    QAction* action = new QAction(tr("Connect"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(connectTo()));
    menu->addAction(action);

    action = new QAction(tr("Settings"), this);
    action->setMenuRole(QAction::PreferencesRole);
    connect(action, SIGNAL(triggered()), qApp, SLOT(showSettings()));
    menu->addAction(action);

    action = new QAction(tr("About %1").arg(Application::applicationName()), this);
    action->setMenuRole(QAction::AboutRole);
    connect(action, SIGNAL(triggered()), qApp, SLOT(aboutApplication()));
    menu->addAction(action);

    action = new QAction(tr("About Qt"), this);
    action->setMenuRole(QAction::AboutQtRole);
    connect(action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    menu->addAction(action);
#endif // Q_WS_MAC

    QSettings settings;
    if (settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());

    applySettings(Application::settings());
    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));

    ConnectionInfos connections = settings.value("connections").value<ConnectionInfos>();
    foreach (const ConnectionInfo& connection, connections)
        connectToImpl(connection);

    if (connections.isEmpty())
        QTimer::singleShot(600, this, SLOT(initialize()));
}

MainWindow::~MainWindow()
{
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

void MainWindow::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
    ConnectionInfo conn;
    conn.host = host;
    conn.port = port;
    conn.nick = nick;
    conn.pass = password;
    connectTo(conn);
}

void MainWindow::connectTo(const ConnectionInfo& connection)
{
    ConnectionWizard wizard;
    wizard.setConnection(connection);

    if (!connection.host.isEmpty() && !connection.nick.isEmpty())
        connectToImpl(connection);
    else if (wizard.exec())
        connectToImpl(wizard.connection());
}

void MainWindow::connectToImpl(const ConnectionInfo& connection)
{
    Session* session = Session::fromConnection(connection, this);
    session->setEncoding(Application::encoding());
    int index = stackView->addSession(session);
    if (!session->hasQuit()) {
        session->open();
        if (!treeWidget->hasRestoredCurrent())
            stackView->setCurrentIndex(index);
    }

    connect(SystemNotifier::instance(), SIGNAL(sleep()), session, SLOT(sleep()));
    connect(SystemNotifier::instance(), SIGNAL(wake()), session, SLOT(wake()));

    connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateOverlay()));
    connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateOverlay()));
    updateOverlay();

    MessageStackView* stack = stackView->sessionWidget(session);
    connect(stack, SIGNAL(viewAdded(MessageView*)), this, SLOT(viewAdded(MessageView*)));
    connect(stack, SIGNAL(viewRemoved(MessageView*)), treeWidget, SLOT(removeView(MessageView*)));
    connect(stack, SIGNAL(viewRenamed(MessageView*)), treeWidget, SLOT(renameView(MessageView*)));
    connect(stack, SIGNAL(viewActivated(MessageView*)), this, SLOT(viewActivated(MessageView*)));

    if (MessageView* view = stack->viewAt(0)) {
        treeWidget->addView(view);
        if (!treeWidget->hasRestoredCurrent() && (!session->hasQuit() || stackView->count() == 1))
            treeWidget->setCurrentView(view);
        treeWidget->parentWidget()->show();
        view->applySettings(Application::settings());
    }

    foreach (const ViewInfo& view, connection.views) {
        if (view.type != -1)
            stack->restoreView(view);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (isVisible()) {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
        settings.setValue("tree", treeWidget->saveState());
        settings.setValue("splitter", static_cast<QSplitter*>(centralWidget())->saveState());

        ConnectionInfos connections;
        QList<Session*> sessions = stackView->sessions();
        foreach (Session* session, sessions) {
            ConnectionInfo connection = session->toConnection();
            connection.views = treeWidget->viewInfos(session);
            connections += connection;
            session->quit();
            session->destructLater();
        }
        settings.setValue("connections", QVariant::fromValue(connections));

        // let the sessions close in the background
        hide();
        event->ignore();
        QTimer::singleShot(1000, this, SLOT(close()));
    } else {
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            if (trayIcon)
                trayIcon->unalert();
            if (dockTile)
                dockTile->setBadge(0);
        }
    }
}

void MainWindow::initialize()
{
    connectTo(ConnectionInfo());
}

void MainWindow::editSession(Session* session)
{
    ConnectionWizard wizard;
    wizard.setConnection(session->toConnection());
    if (wizard.exec())
        session->initFrom(wizard.connection());
}

void MainWindow::applySettings(const Settings& settings)
{
    searchShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::SearchView)));

    foreach (MessageView* view, findChildren<MessageView*>())
        view->applySettings(settings);
    treeWidget->applySettings(settings);
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
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

void MainWindow::highlighted(IrcMessage* message)
{
    Q_UNUSED(message);
    if (!isActiveWindow()) {
        QApplication::alert(this);
        if (trayIcon)
            trayIcon->alert();
        if (dockTile)
            dockTile->setBadge(dockTile->badge() + 1);
    }

    MessageView* view = qobject_cast<MessageView*>(sender());
    if (view) {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != ViewInfo::Server)
            item = item->findChild(view->receiver());
        if (item) {
            treeWidget->highlight(item);
            item->setBadge(item->badge() + 1);
        }
    }
}

void MainWindow::missed(IrcMessage* message)
{
    Q_UNUSED(message);
    MessageView* view = qobject_cast<MessageView*>(sender());
    if (view) {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != ViewInfo::Server)
            item = item->findChild(view->receiver());
        if (item)
            item->setBadge(item->badge() + 1);
    }
}

void MainWindow::viewAdded(MessageView* view)
{
    connect(view, SIGNAL(splitterChanged(QByteArray)), this, SLOT(splitterChanged(QByteArray)));
    connect(view, SIGNAL(highlighted(IrcMessage*)), this, SLOT(highlighted(IrcMessage*)));
    connect(view, SIGNAL(missed(IrcMessage*)), this, SLOT(missed(IrcMessage*)));

    view->applySettings(Application::settings());

    QSettings settings;
    if (settings.contains("list"))
        view->restoreSplitter(settings.value("list").toByteArray());

    Session* session = view->session();
    treeWidget->addView(view);
    if (settings.contains("tree"))
        treeWidget->restoreState(settings.value("tree").toByteArray());
    treeWidget->expandItem(treeWidget->sessionItem(session));
}

void MainWindow::viewActivated(MessageView* view)
{
    QSettings settings;
    if (settings.contains("list"))
        view->restoreSplitter(settings.value("list").toByteArray());

    treeWidget->setCurrentView(view);
}

void MainWindow::closeTreeItem(SessionTreeItem* item)
{
    MessageStackView* stack = stackView->sessionWidget(item->session());
    if (stack) {
        int index = stack->indexOf(item->view());
        stack->closeView(index);
        if (index == 0) {
            stackView->removeSession(stack->session());
            treeWidget->parentWidget()->setVisible(!stackView->sessions().isEmpty());
        }
    }
}

void MainWindow::currentTreeItemChanged(Session* session, const QString& view)
{
    MessageStackView* stack = stackView->sessionWidget(session);
    if (stack) {
        stackView->setCurrentWidget(stack);
        if (view.isEmpty())
            stack->setCurrentIndex(0);
        else
            stack->openView(view);
    }
    setWindowFilePath(view);
    updateOverlay();
}

void MainWindow::splitterChanged(const QByteArray& state)
{
    QSettings settings;
    settings.setValue("list", state);
}

void MainWindow::updateOverlay()
{
    MessageStackView* stack = stackView->currentWidget();
    if (stack && stack->session()) {
        if (!overlay) {
            overlay = new Overlay(stackView);
            connect(overlay, SIGNAL(refresh()), this, SLOT(reconnectSession()));
        }
        Session* session = stack->session();
        overlay->setParent(stack->currentWidget());
        overlay->setBusy(session->isActive() && !session->isConnected());
        overlay->setRefresh(!session->isActive());
        overlay->setVisible(!session->isConnected());
    }
}

void MainWindow::reconnectSession()
{
    MessageStackView* stack = stackView->currentWidget();
    if (stack)
        stack->session()->reconnect();
}

void MainWindow::addView()
{
    MessageStackView* stack = stackView->currentWidget();
    if (stack && stack->session()->isActive()) {
        AddViewDialog dialog(stack->session(), this);
        if (dialog.exec()) {
            QString view = dialog.view();
            if (stack->session()->isChannel(view))
                stack->session()->sendCommand(IrcCommand::createJoin(view, dialog.password()));
            stack->openView(view);
        }
    }
}

void MainWindow::closeView()
{
    MessageStackView* stack = stackView->currentWidget();
    if (stack) {
        int index = stack->currentIndex();
        stack->closeView(index);
        if (index == 0) {
            stackView->removeSession(stack->session());
            treeWidget->parentWidget()->setVisible(!stackView->sessions().isEmpty());
        }
    }
}

void MainWindow::searchView()
{
    SearchPopup* search = new SearchPopup(this);
    connect(search, SIGNAL(searched(QString)), treeWidget, SLOT(search(QString)));
    connect(search, SIGNAL(searchedAgain(QString)), treeWidget, SLOT(searchAgain(QString)));
    connect(search, SIGNAL(destroyed()), treeWidget, SLOT(unblockItemReset()));
    connect(treeWidget, SIGNAL(searched(bool)), search, SLOT(onSearched(bool)));
    treeWidget->blockItemReset();
    search->popup();
}

void MainWindow::createTree()
{
    QSplitter* splitter = static_cast<QSplitter*>(centralWidget());
    QWidget* container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    container->setVisible(false);

    treeWidget = new SessionTreeWidget(container);
    treeWidget->setFocusPolicy(Qt::NoFocus);

    connect(treeWidget, SIGNAL(editSession(Session*)), this, SLOT(editSession(Session*)));
    connect(treeWidget, SIGNAL(closeItem(SessionTreeItem*)), this, SLOT(closeTreeItem(SessionTreeItem*)));
    connect(treeWidget, SIGNAL(currentViewChanged(Session*, QString)), this, SLOT(currentTreeItemChanged(Session*, QString)));

    ToolBar* toolBar = new ToolBar(container);
    connect(toolBar, SIGNAL(aboutTriggered()), qApp, SLOT(aboutApplication()));
    connect(toolBar, SIGNAL(settingsTriggered()), qApp, SLOT(showSettings()));
    connect(toolBar, SIGNAL(connectTriggered()), this, SLOT(connectTo()));
    connect(toolBar, SIGNAL(joinTriggered()), this, SLOT(addView()));

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(treeWidget);
    layout->addWidget(toolBar);
    layout->setSpacing(0);
    layout->setMargin(0);

    container->setMinimumWidth(toolBar->sizeHint().width());
    splitter->insertWidget(0, container);
    splitter->setStretchFactor(1, 1);
    QSettings settings;
    if (settings.contains("splitter"))
        splitter->restoreState(settings.value("splitter").toByteArray());
}
