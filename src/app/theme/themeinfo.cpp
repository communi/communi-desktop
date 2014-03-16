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

#include "themeinfo.h"
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QDir>

bool ThemeInfo::isValid() const
{
    return !d.name.isEmpty();
}

QString ThemeInfo::name() const
{
    return d.name;
}

QString ThemeInfo::author() const
{
    return d.author;
}

QString ThemeInfo::version() const
{
    return d.version;
}

QString ThemeInfo::description() const
{
    return d.description;
}

QString ThemeInfo::style() const
{
    return d.style;
}

static QString readFile(const QDir& dir, const QString& fileName)
{
    QFile file;
    if (QFileInfo(fileName).isRelative())
        file.setFileName(dir.filePath(fileName));
    else
        file.setFileName(fileName);
    if (file.open(QFile::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(file.readAll());
    return QString();
}

bool ThemeInfo::load(const QString& filePath)
{
    QSettings settings(filePath, QSettings::IniFormat);

    QStringList groups = settings.childGroups();
    if (groups.contains("Theme")) {
        settings.beginGroup("Theme");
        d.name = settings.value("name").toString();
        d.author = settings.value("author").toString();
        d.version = settings.value("version").toString();
        d.description = settings.value("description").toString();
        d.style = readFile(QFileInfo(filePath).dir(), settings.value("style").toString());
        settings.endGroup();
    }
    return isValid();
}
