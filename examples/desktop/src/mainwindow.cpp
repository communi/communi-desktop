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
#include "connectioninfo.h"
#include "homepage.h"
#include "session.h"
#include "qtdocktile.h"
#include <QtGui>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), tabWidget(0), trayIcon(0), dockTile(0)
{
    tabWidget = new MultiSessionTabWidget(this);
    tabWidget->applySettings(Application::settings());
    connect(qApp, SIGNAL(settingsChanged(Settings)), tabWidget, SLOT(applySettings(Settings)));
    setCentralWidget(tabWidget);
    connect(tabWidget, SIGNAL(newTabRequested()), this, SLOT(connectTo()), Qt::QueuedConnection);
    connect(tabWidget, SIGNAL(alertStatusChanged(bool)), this, SLOT(activateAlert(bool)));

    HomePage* homePage = new HomePage(this);
    connect(homePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
    tabWidget->addTab(homePage, tr("Home"));

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = new TrayIcon(this);
        trayIcon->setIcon(QApplication::windowIcon());
        trayIcon->setVisible(true);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }

    if (QtDockTile::isAvailable())
    {
        dockTile = new QtDockTile(this);
        dockTile->setBadge("3");
    }

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
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());

    ConnectionInfos connections;
    if (tabWidget)
    {
        QList<Session*> sessions = tabWidget->sessions();
        foreach (Session* session, sessions)
        {
            connections += session->toConnection();
            session->quit();
        }
    }
    settings.setValue("connections", QVariant::fromValue(connections));

    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::ActivationChange)
    {
        if (trayIcon && isActiveWindow())
            trayIcon->unalert();
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

void MainWindow::activateAlert(bool activate)
{
    if (!isActiveWindow())
    {
        if (activate)
        {
            if (trayIcon)
                trayIcon->alert();
            QApplication::alert(this);
        }
        else
        {
            if (trayIcon)
                trayIcon->unalert();
        }
    }
}
