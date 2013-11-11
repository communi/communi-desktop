/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class IrcBuffer;
class IrcConnection;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget* parent = 0);
    ~Window();

    virtual QList<IrcConnection*> connections() const = 0;
    virtual void addConnection(IrcConnection* connection) = 0;
    virtual void removeConnection(IrcConnection* connection) = 0;
};

#endif // WINDOW_H
