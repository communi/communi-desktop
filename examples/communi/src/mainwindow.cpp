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

#include "mainwindow.h"
#include "application.h"
#include "connectionwizard.h"
#include "sessiontabwidget.h"
#include "maintabwidget.h"
#include "sharedtimer.h"
#include "welcomepage.h"
#include "connection.h"
#include "session.h"
#include <QtGui>

#ifdef Q_WS_MAEMO_5
#include <mce/dbus-names.h>
#include <QtDBus>
#endif // Q_WS_MAEMO_5
#include <ircmessage.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), tabWidget(0)
#ifndef Q_OS_SYMBIAN
    , trayIcon(0)
#endif // Q_OS_SYMBIAN
{
    createWelcomeView();

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO5)
    setAttribute(Qt::WA_LockLandscapeOrientation, true);
#endif

#ifndef Q_OS_SYMBIAN
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = new TrayIcon(this);
        trayIcon->setIcon(QApplication::windowIcon());
        trayIcon->setVisible(true);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }
#endif // Q_OS_SYMBIAN

#ifdef Q_WS_MAEMO_5
    interface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
                                   QDBusConnection::systemBus(), this);

    QDBusMessage reply = interface->call(MCE_ENABLE_VIBRATOR);
    if (reply.type() == QDBusMessage::ErrorMessage)
        qDebug() << reply.errorMessage();
#endif // Q_WS_MAEMO_5

    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q")), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

#if defined(Q_WS_MAC) || defined(Q_WS_MAEMO_5)
    QMenu* menu = new QMenu(this);
    menuBar()->addMenu(menu);

    QAction* action = new QAction(tr("Connect"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(connectTo()));
    menu->addAction(action);

    action = new QAction(tr("Settings"), this);
    action->setMenuRole(QAction::PreferencesRole);
    connect(action, SIGNAL(triggered()), qApp, SLOT(showSettings()));
    menu->addAction(action);

#ifdef Q_WS_MAEMO_5
    QMenu* tabsMenu = new QMenu(tr("Tabs"), this);
    menu->addMenu(tabsMenu);

    networksAction = new QAction(tr("Networks"), this);
    networksAction->setCheckable(true);
    networksAction->setChecked(true);
    tabsMenu->addAction(networksAction);

    channelsAction = new QAction(tr("Channels"), this);
    channelsAction->setCheckable(true);
    channelsAction->setChecked(true);
    tabsMenu->addAction(channelsAction);
#endif // Q_WS_MAEMO_5

    action = new QAction(tr("About %1").arg(Application::applicationName()), this);
    action->setMenuRole(QAction::AboutRole);
    connect(action, SIGNAL(triggered()), qApp, SLOT(aboutApplication()));
    menu->addAction(action);

#ifdef Q_WS_MAEMO_5
    action = new QAction(tr("About Oxygen"), this);
    connect(action, SIGNAL(triggered()), qApp, SLOT(aboutOxygen()));
    menu->addAction(action);
#endif // Q_WS_MAEMO_5

    action = new QAction(tr("About Qt"), this);
    action->setMenuRole(QAction::AboutQtRole);
    connect(action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    menu->addAction(action);
#endif // Q_WS_MAC || Q_WS_MAEMO_5

    QSettings settings;
    if (settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());

    QTimer::singleShot(1000, this, SLOT(initialize()));
}

MainWindow::~MainWindow()
{
#ifdef Q_WS_MAEMO_5
    QDBusMessage reply = interface->call(MCE_DISABLE_VIBRATOR);
    if (reply.type() == QDBusMessage::ErrorMessage)
        qDebug() << reply.errorMessage();
#endif // Q_WS_MAEMO_5
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

void MainWindow::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
    Connection conn;
    conn.host = host;
    conn.port = port;
    conn.nick = nick;
    conn.pass = password;
    connectTo(conn);
}

void MainWindow::connectTo(const Connection& connection)
{
    ConnectionWizard wizard;
    wizard.setConnection(connection);

    if (!connection.host.isEmpty() && !connection.nick.isEmpty())
        connectToImpl(connection);
    else if (wizard.exec())
        connectToImpl(wizard.connection());
}

void MainWindow::connectToImpl(const Connection& connection)
{
    if (!tabWidget)
        createTabbedView();

    Session* session = new Session(this);
    session->connectTo(connection);

    SessionTabWidget* tab = new SessionTabWidget(session, tabWidget);
    connect(tab, SIGNAL(disconnectFrom(QString)), this, SLOT(disconnectFrom(QString)));
    if (connection.name.isEmpty())
        connect(tab, SIGNAL(titleChanged(QString)), tabWidget, SLOT(setSessionTitle(QString)));
    connect(tab, SIGNAL(alertStatusChanged(bool)), tabWidget, SLOT(activateAlert(bool)));
    connect(tab, SIGNAL(highlightStatusChanged(bool)), tabWidget, SLOT(activateHighlight(bool)));
    connect(tab, SIGNAL(vibraRequested(bool)), this, SLOT(activateVibra(bool)));
#ifdef Q_WS_MAEMO_5
    connect(channelsAction, SIGNAL(toggled(bool)), tab, SLOT(setTabBarVisible(bool)));
    tab->setTabBarVisible(channelsAction->isChecked());
#endif // Q_WS_MAEMO_5
    tabWidget->addTab(tab, connection.name.isEmpty() ? session->host() : connection.name);
}

void MainWindow::disconnectFrom(const QString& message)
{
    SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());
    if (!tab)
        tab = qobject_cast<SessionTabWidget*>(tabWidget->currentWidget());
    Q_ASSERT(tab);
    QString reason = message.trimmed();
    if (reason.isEmpty())
        reason = tr("%1 %2 - %3").arg(Application::applicationName())
        .arg(Application::applicationVersion())
        .arg(Application::organizationDomain());
    IrcQuitMessage msg;
    msg.setReason(reason);
    tab->session()->sendMessage(&msg);
    tab->session()->close();
    // automatically rejoin channels when reconnected
    tab->session()->setAutoJoinChannels(tab->session()->connection().channels);
}

void MainWindow::quit(const QString& message)
{
    // TODO: confirm?
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidget->widget(i));
        QString reason = message.trimmed();
        if (reason.isEmpty())
            reason = tr("%1 %2 - %3").arg(Application::applicationName())
                                     .arg(Application::applicationVersion())
                                     .arg(Application::organizationDomain());
        IrcQuitMessage msg;
        msg.setReason(reason);
        tab->session()->sendMessage(&msg);
        tab->session()->close();
    }
    close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());

    Connections connections;
    foreach (Session* session, Application::sessions())
        connections += session->connection();
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
#ifndef Q_OS_SYMBIAN
            if (trayIcon)
                trayIcon->unalert();
#endif // Q_OS_SYMBIAN
            if (tabWidget)
            {
                SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidget->currentWidget());
                if (tab)
                    QMetaObject::invokeMethod(tab, "delayedTabReset");
            }
        }
    }
}

bool MainWindow::event(QEvent* event)
{
#ifdef Q_WS_MAEMO_5
    switch (event->type())
    {
        case QEvent::Show:
        case QEvent::Enter: // needed by Maemo
        case QEvent::WindowActivate: // needed by Symbian
            SharedTimer::instance()->resume();
            break;
        case QEvent::Hide:
        case QEvent::Leave: // needed by Maemo
        case QEvent::WindowDeactivate: // needed by Symbian
            SharedTimer::instance()->pause();
            break;
        default:
            break;
    }
#endif // Q_WS_MAEMO_5
    return QMainWindow::event(event);
}

void MainWindow::initialize()
{
    QSettings settings;
    Connections connections = settings.value("connections").value<Connections>();

    foreach (const Connection& connection, connections)
        connectToImpl(connection);

    if (connections.isEmpty())
        connectTo(Connection());
}

#ifndef Q_OS_SYMBIAN
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
#endif // Q_OS_SYMBIAN

void MainWindow::activateAlert(bool activate)
{
    if (!isActiveWindow())
    {
        if (activate)
        {
#ifndef Q_OS_SYMBIAN
            if (trayIcon)
                trayIcon->alert();
#endif // Q_OS_SYMBIAN
            QApplication::alert(this);
        }
        else
        {
#ifndef Q_OS_SYMBIAN
            if (trayIcon)
                trayIcon->unalert();
#endif Q_OS_SYMBIAN
        }
    }
}

void MainWindow::activateVibra(bool activate)
{
    Q_UNUSED(activate);
#ifdef Q_WS_MAEMO_5
    QDBusMessage reply = interface->call(activate ? MCE_ACTIVATE_VIBRATOR_PATTERN : MCE_DEACTIVATE_VIBRATOR_PATTERN,
                                         QLatin1String("PatternChatAndEmail"));
    if (reply.type() == QDBusMessage::ErrorMessage)
        qDebug() << reply.errorMessage();
#endif // Q_WS_MAEMO_5
}

void MainWindow::tabActivated(int index)
{
    if (index == -1)
    {
        createWelcomeView();
    }
    else if (QTabWidget* tab = qobject_cast<QTabWidget*>(tabWidget->widget(index)))
    {
        setWindowFilePath(tab->tabText(tab->currentIndex()));
        QMetaObject::invokeMethod(tab, "delayedTabReset");
    }
}

void MainWindow::createWelcomeView()
{
    WelcomePage* welcomePage = new WelcomePage(this);
    connect(welcomePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
    setCentralWidget(welcomePage);
    tabWidget = 0;
}

void MainWindow::createTabbedView()
{
    tabWidget = new MainTabWidget(this);
    setCentralWidget(tabWidget);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(tabWidget, SIGNAL(alertStatusChanged(bool)), this, SLOT(activateAlert(bool)));
#ifdef Q_WS_MAEMO_5
    connect(networksAction, SIGNAL(toggled(bool)), tabWidget, SLOT(setTabBarVisible(bool)));
#endif // Q_WS_MAEMO_5
}
