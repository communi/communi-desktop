/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "mainwindow.h"
#include "settingspage.h"
#include "systemmonitor.h"
#include "pluginloader.h"
#include "textdocument.h"
#include "connectpage.h"
#include "bufferview.h"
#include "helppopup.h"
#include "chatpage.h"
#include "dock.h"
#include <IrcCommandQueue>
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
    d.save = false;

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
    connect(d.chatPage, SIGNAL(alert(IrcMessage*)), d.dock, SLOT(alert(IrcMessage*)));

    d.monitor = new SystemMonitor(this);
    connect(d.monitor, SIGNAL(screenLocked()), d.dock, SLOT(activateAlert()));
    connect(d.monitor, SIGNAL(screenUnlocked()), d.dock, SLOT(deactivateAlert()));
    connect(d.monitor, SIGNAL(screenSaverStarted()), d.dock, SLOT(activateAlert()));
    connect(d.monitor, SIGNAL(screenSaverStopped()), d.dock, SLOT(deactivateAlert()));

    PluginLoader::instance()->windowCreated(this);

    restoreState();
    d.save = true;

    if (d.connections.isEmpty())
        doConnect();
}

MainWindow::~MainWindow()
{
    PluginLoader::instance()->windowDestroyed(this);
}

void MainWindow::saveState()
{
    if (!d.save)
        return;

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
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
        if (model)
            model->restoreState(state.value("model").toByteArray());
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

    connection->setReconnectDelay(15);
    connection->network()->setRequestedCapabilities(Irc::supportedCapabilities());

    IrcCommandQueue* queue = new IrcCommandQueue(connection);
    queue->setConnection(connection);

    // backwards compatibility
    if (connection->nickNames().isEmpty())
        connection->setNickNames(QStringList() << connection->nickName());
    if (connection->servers().isEmpty())
        connection->setServers(QStringList() << QString("%1 %2%3").arg(connection->host())
                                                                  .arg(connection->isSecure() ? "+" : "")
                                                                  .arg(connection->port()));

    connect(d.monitor, SIGNAL(wake()), connection, SLOT(open()));
    connect(d.monitor, SIGNAL(online()), connection, SLOT(open()));

    connect(d.monitor, SIGNAL(sleep()), connection, SLOT(quit()));
    connect(d.monitor, SIGNAL(sleep()), connection, SLOT(close()));

    connect(d.monitor, SIGNAL(offline()), connection, SLOT(close()));

    d.connections += connection;
    connect(connection, SIGNAL(enabledChanged(bool)), this, SLOT(saveState()));
    connect(connection, SIGNAL(destroyed(IrcConnection*)), this, SLOT(removeConnection(IrcConnection*)));
    emit connectionAdded(connection);

    saveState();
}

void MainWindow::removeConnection(IrcConnection* connection)
{
    if (d.connections.removeOne(connection))
        emit connectionRemoved(connection);
    if (d.connections.isEmpty())
        doConnect();
    else
        saveState();
}

void MainWindow::push(QWidget* page)
{
    QWidget* prev = d.stack->currentWidget();
    if (prev) {
        prev->setProperty("__focus_widget__", QVariant::fromValue(prev->focusWidget()));
        prev->setEnabled(false);
    }
    d.stack->addWidget(page);
    d.stack->setCurrentWidget(page);
    page->setFocus();
}

void MainWindow::pop()
{
    QWidget* page = d.stack->currentWidget();
    if (!qobject_cast<ChatPage*>(page)) {
        d.stack->removeWidget(page);
        d.stack->setCurrentIndex(d.stack->count() - 1);
        QWidget* current = d.stack->currentWidget();
        if (current) {
            current->setEnabled(true);
            QWidget* fw = current->property("__focus_widget__").value<QWidget*>();
            if (fw)
                fw->setFocus();
        }
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
        d.save = false;

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
        connection->setServers(page->servers());
        connection->setNickNames(page->nickNames());
        connection->setRealName(page->realName());
        connection->setUserName(page->userName());
        connection->setDisplayName(page->displayName());
        connection->setPassword(page->password());
        connection->setSaslMechanism(page->saslMechanism());
        saveState();
        pop();
    }
}

void MainWindow::onConnectAccepted()
{
    ConnectPage* page = qobject_cast<ConnectPage*>(sender());
    if (page) {
        IrcConnection* connection = new IrcConnection(d.chatPage);
        connection->setServers(page->servers());
        connection->setNickNames(page->nickNames());
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
        saveState();
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
    page->setServers(connection->servers());
    page->setNickNames(connection->nickNames());
    page->setRealName(connection->realName());
    page->setUserName(connection->userName());
    page->setDisplayName(connection->displayName());
    page->setPassword(connection->password());
    page->setSaslMechanism(connection->saslMechanism());
    push(page);
}
