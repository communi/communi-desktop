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

#ifndef SYSTEMNOTIFIER_H
#define SYSTEMNOTIFIER_H

#include <QObject>

class SystemNotifierPrivate;

class SystemNotifier : public QObject
{
    Q_OBJECT

public:
    static SystemNotifier* instance();

signals:
    void wake();
    void sleep();
    void online();
    void offline();

protected:
    SystemNotifier(QObject* parent = 0);
    ~SystemNotifier();

    void initialize();
    void uninitialize();

private:
    SystemNotifierPrivate* d;
};

#endif // SYSTEMNOTIFIER_H
