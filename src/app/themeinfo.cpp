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

#include "themeinfo.h"
#include <QStringList>
#include <QSettings>
#include <QFile>

QString ThemeInfo::attribute(const QString& key) const
{
    return d.attributes.value(key);
}

static QString readFile(const QString& filePath)
{
    QFile file(filePath);
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
        foreach (const QString& key, settings.childKeys())
            d.attributes.insert(key, settings.value(key).toString());
        d.attributes.insert("document", readFile(settings.value("document").toString()));
        d.attributes.insert("application", readFile(settings.value("application").toString()));
        settings.endGroup();
    }
    return !d.attributes.value("name").isEmpty();
}
