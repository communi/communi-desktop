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

#include "trayicon.h"
#include <QTimer>

TrayIcon::TrayIcon(QObject* parent) : QSystemTrayIcon(parent)
{
    d.timer = 0;
}

void TrayIcon::alert()
{
    if (!d.timer)
    {
        d.icon = icon();
        d.timer = new QTimer(this);
        connect(d.timer, SIGNAL(timeout()), this, SLOT(toggleIcon()));
        d.timer->start(500);
    }
}

void TrayIcon::unalert()
{
    if (d.timer)
    {
        d.timer->stop();
        d.timer->deleteLater();
        d.timer = 0;
        setIcon(d.icon);
    }
}

void TrayIcon::toggleIcon()
{
    if (icon().isNull())
        setIcon(d.icon);
    else
        setIcon(QIcon());
}
