/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "mainwindow.h"
#include <IrcConnection>
#include <QUuid>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
}

MainWindow::~MainWindow()
{
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
