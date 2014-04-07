/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>

class SystemMonitorPrivate;

class SystemMonitor : public QObject
{
    Q_OBJECT

public:
    SystemMonitor(QObject* parent = 0);
    ~SystemMonitor();

signals:
    void wake();
    void sleep();
    void online();
    void offline();

protected:
    void initialize();
    void uninitialize();

private:
    SystemMonitorPrivate* d;
};

#endif // SYSTEMMONITOR_H
