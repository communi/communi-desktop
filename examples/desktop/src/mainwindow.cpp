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

#include "mainwindow.h"
#include "application.h"
#include "connectionwizard.h"
#include "multisessiontabwidget.h"
#include "sessiontreewidget.h"
#include "sessiontabwidget.h"
#include "sessiontreeitem.h"
#include "connectioninfo.h"
#include "messageview.h"
#include "homepage.h"
#include "session.h"
#include "qtdocktile.h"
#include <QSettings>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    toolBar(0), homePage(0), treeWidget(0), trayIcon(0), dockTile(0)
{
    tabWidget = new MultiSessionTabWidget(this);

    connect(tabWidget, SIGNAL(newTabRequested()), this, SLOT(connectTo()), Qt::QueuedConnection);
    connect(tabWidget, SIGNAL(alerted(MessageView*,IrcMessage*)), this, SLOT(alert(MessageView*,IrcMessage*)));
    connect(tabWidget, SIGNAL(highlighted(MessageView*,IrcMessage*)), this, SLOT(highlight(MessageView*,IrcMessage*)));
    connect(tabWidget, SIGNAL(splitterChanged(QByteArray)), this, SLOT(splitterChanged(QByteArray)));

    QSplitter* splitter = new QSplitter(this);
    splitter->setHandleWidth(1);
    splitter->addWidget(tabWidget);
    setCentralWidget(splitter);

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = new TrayIcon(this);
        trayIcon->setIcon(QApplication::windowIcon());
        trayIcon->setVisible(true);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }

    if (QtDockTile::isAvailable())
        dockTile = new QtDockTile(this);

    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q")), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

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

    QTimer::singleShot(1000, this, SLOT(initialize()));
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
    session->setUserName("communi");
    if (session->ensureNetwork())
        session->open();
    tabWidget->addSession(session);

    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    connect(tab, SIGNAL(viewAdded(MessageView*)), this, SLOT(viewAdded(MessageView*)));
    connect(tab, SIGNAL(viewRemoved(MessageView*)), this, SLOT(viewRemoved(MessageView*)));
    connect(tab, SIGNAL(viewRenamed(QString,QString)), this, SLOT(viewRenamed(QString,QString)));
    connect(tab, SIGNAL(viewActivated(MessageView*)), this, SLOT(viewActivated(MessageView*)));

    QSettings settings;
    if (settings.contains("list"))
        tab->restoreSplitter(settings.value("list").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    if (treeWidget)
        settings.setValue("tree", static_cast<QSplitter*>(centralWidget())->saveState());

    ConnectionInfos connections;
    QList<Session*> sessions = tabWidget->sessions();
    foreach (Session* session, sessions)
    {
        connections += session->toConnection();
        session->quit();
    }
    settings.setValue("connections", QVariant::fromValue(connections));

    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::ActivationChange)
    {
        if (isActiveWindow())
        {
            if (trayIcon)
                trayIcon->unalert();
            if (dockTile)
                dockTile->setBadge(0);
        }
    }
}

void MainWindow::initialize()
{
    QSettings settings;
    ConnectionInfos connections = settings.value("connections").value<ConnectionInfos>();

    foreach (const ConnectionInfo& connection, connections)
        connectToImpl(connection);

    if (connections.isEmpty())
        connectTo(ConnectionInfo());
}

void MainWindow::applySettings(const Settings& settings)
{
    tabWidget->applySettings(settings);
    if (settings.layout == "tree")
    {
        if (!treeWidget)
            createTree();
        treeWidget->applySettings(settings);

        homePage->deleteLater();
        homePage = 0;
    }
    else if (treeWidget)
    {
        if (!homePage)
            createHome();

        treeWidget->parentWidget()->deleteLater();
        treeWidget = 0;
        toolBar = 0;
    }
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

void MainWindow::alert(MessageView* view, IrcMessage* message)
{
    Q_UNUSED(message);
    if (!isActiveWindow())
    {
        QApplication::alert(this);
        if (trayIcon)
            trayIcon->alert();
        if (dockTile)
            dockTile->setBadge(dockTile->badge() + 1);
    }

    if (treeWidget)
    {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != MessageView::ServerView)
            item = item->findChild(view->receiver());
        if (item) {
            item->setAlerted(true);
            treeWidget->alert(item);
        }
    }
}

void MainWindow::highlight(MessageView* view, IrcMessage* message)
{
    Q_UNUSED(message);
    if (treeWidget)
    {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != MessageView::ServerView)
            item = item->findChild(view->receiver());
        if (item)
            item->setHighlighted(true);
    }
}

void MainWindow::viewAdded(MessageView* view)
{
    QSettings settings;
    if (settings.contains("list"))
        view->restoreSplitter(settings.value("list").toByteArray());

    if (treeWidget)
    {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());
        if (tab)
        {
            Session* session = tab->session();
            treeWidget->addView(session, view->receiver());
            treeWidget->expandItem(treeWidget->sessionItem(session));
        }
    }
}

void MainWindow::viewRemoved(MessageView* view)
{
    if (treeWidget)
    {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());
        if (tab)
            treeWidget->removeView(tab->session(), view->receiver());
    }
}

void MainWindow::viewRenamed(const QString& from, const QString& to)
{
    if (treeWidget)
    {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());
        if (tab)
            treeWidget->renameView(tab->session(), from, to);
    }
}

void MainWindow::viewActivated(MessageView *view)
{
    if (treeWidget)
    {
        QString receiver;
        if (view->viewType() != MessageView::ServerView)
            receiver = view->receiver();
        treeWidget->setCurrentView(view->session(), receiver);
    }
}

void MainWindow::currentTreeItemChanged(Session* session, const QString& view)
{
    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    if (tab)
    {
        tabWidget->setCurrentWidget(tab);
        if (view.isEmpty())
            tab->setCurrentIndex(0);
        else
            tab->openView(view);
    }
}

void  MainWindow::menuRequested(SessionTreeItem* item, const QPoint& pos)
{
    SessionTreeItem* parent = static_cast<SessionTreeItem*>(item->parent());
    if (parent)
    {
        int index = treeWidget->indexOfTopLevelItem(parent) + 1;
        QMetaObject::invokeMethod(tabWidget->widget(index), "onTabMenuRequested", Q_ARG(int, parent->indexOfChild(item) + 1), Q_ARG(QPoint, pos));
    }
    else
    {
        int index = treeWidget->indexOfTopLevelItem(item) + 1;
        QMetaObject::invokeMethod(tabWidget, "onTabMenuRequested", Q_ARG(int, index), Q_ARG(QPoint, pos));
    }
}

void MainWindow::splitterChanged(const QByteArray& state)
{
    QSettings settings;
    settings.setValue("list", state);
}

void MainWindow::addView()
{
    SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidget->currentWidget());
    if (tab)
        QMetaObject::invokeMethod(tab, "onNewTabRequested");
}

void MainWindow::createTree()
{
    QSplitter* splitter = static_cast<QSplitter*>(centralWidget());
    QWidget* container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    treeWidget = new SessionTreeWidget(container);
    treeWidget->setFocusPolicy(Qt::NoFocus);

    connect(treeWidget, SIGNAL(currentViewChanged(Session*,QString)), this, SLOT(currentTreeItemChanged(Session*,QString)));
    connect(treeWidget, SIGNAL(menuRequested(SessionTreeItem*,QPoint)), this, SLOT(menuRequested(SessionTreeItem*,QPoint)));

    connect(tabWidget, SIGNAL(sessionAdded(Session*)), treeWidget, SLOT(addSession(Session*)));
    connect(tabWidget, SIGNAL(sessionRemoved(Session*)), treeWidget, SLOT(removeSession(Session*)));

    foreach (Session* session, tabWidget->sessions())
    {
        treeWidget->addSession(session);
        SessionTabWidget* tab = tabWidget->sessionWidget(session);
        for (int i = 1; i < tab->count() - 1; ++i)
        {
            MessageView* view = qobject_cast<MessageView*>(tab->widget(i));
            if (view)
                treeWidget->addView(session, view->receiver());
        }
    }
    treeWidget->expandAll();

    toolBar = new QToolBar(container);
    toolBar->setIconSize(QSize(12, 12));
    toolBar->addAction(QIcon(":/resources/iconmonstr/about.png"), "", qApp, SLOT(aboutApplication()))->setToolTip(tr("About"));
    toolBar->addAction(QIcon(":/resources/iconmonstr/settings.png"), "", qApp, SLOT(showSettings()))->setToolTip(tr("Settings"));
    QWidget* spacer = new QWidget(toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);
    toolBar->addAction(QIcon(":/resources/iconmonstr/connect.png"), "", this, SLOT(connectTo()))->setToolTip(tr("Connect"));
    toolBar->addAction(QIcon(":/resources/iconmonstr/new-view.png"), "", this, SLOT(addView()))->setToolTip(tr("Join channel"));

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(treeWidget);
    layout->addWidget(toolBar);
    layout->setSpacing(0);
    layout->setMargin(0);

    container->setMinimumWidth(toolBar->sizeHint().width());
    splitter->insertWidget(0, container);
    splitter->setStretchFactor(1, 1);
    QSettings settings;
    if (settings.contains("tree"))
        splitter->restoreState(settings.value("tree").toByteArray());
}

void MainWindow::createHome()
{
    homePage = new HomePage(tabWidget);
    connect(homePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
    tabWidget->insertTab(0, homePage, tr("Home"));
}
