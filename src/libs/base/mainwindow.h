/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

class BufferView;
class IrcConnection;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    BufferView* currentView() const;
    QList<IrcConnection*> connections() const;

public slots:
    void setCurrentView(BufferView* view);
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

signals:
    void currentViewChanged(BufferView* view);
    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

private:
    struct Private {
        QPointer<BufferView> view;
        QList<IrcConnection*> connections;
    } d;
};

#endif // MAINWINDOW_H
