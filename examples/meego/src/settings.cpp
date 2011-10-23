/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#include "settings.h"
#include <QSettings>
#include <QTime>

Settings::Settings(QObject *parent) : QObject(parent)
{
    qsrand(QTime::currentTime().msec());
}

QString Settings::host() const
{ return QSettings().value("host", QString("irc.freenode.net")).toString(); }

void Settings::setHost(const QString& host)
{ QSettings().setValue("host", host); }

int Settings::port() const
{ return QSettings().value("port", 6667).toInt(); }

void Settings::setPort(int port)
{ QSettings().setValue("port", port); }

bool Settings::isSecure() const
{ return QSettings().value("secure", false).toBool(); }

void Settings::setSecure(bool secure)
{ QSettings().setValue("secure", secure); }

QString Settings::name() const
{ return QSettings().value("name", QString("Guest%1").arg(qrand() % 9999)).toString(); }

void Settings::setName(const QString& name)
{ QSettings().setValue("name", name); }

QString Settings::channel() const
{ return QSettings().value("channel", QString("#communi")).toString(); }

void Settings::setChannel(const QString& channel)
{ QSettings().setValue("channel", channel); }
