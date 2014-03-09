/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "mainwindow.h"
#include "bufferview.h"
#include <IrcConnection>
#include <QUuid>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    d.view = 0;
}

MainWindow::~MainWindow()
{
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
    if (!connection->userData().isValid())
        connection->setUserData(QUuid::createUuid().toString());

    d.connections += connection;
    connect(connection, SIGNAL(destroyed(IrcConnection*)), this, SLOT(removeConnection(IrcConnection*)));
    emit connectionAdded(connection);
}

void MainWindow::removeConnection(IrcConnection* connection)
{
    if (d.connections.removeOne(connection))
        emit connectionRemoved(connection);
}
