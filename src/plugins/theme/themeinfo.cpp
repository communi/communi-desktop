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
#include "styleparser.h"
#include <QApplication>
#include <QSettings>
#include <QPalette>
#include <QColor>
#include <QFile>
#include <Irc>

QString ThemeInfo::attribute(const QString& key) const
{
    return d.attributes.value(key);
}

QString ThemeInfo::prefix() const
{
    return d.prefix;
}

QString ThemeInfo::docStyleSheet() const
{
    return d.css;
}

QString ThemeInfo::appStyleSheet() const
{
    return d.qss;
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
        settings.endGroup();
    }

    if (groups.contains("Resources")) {
        settings.beginGroup("Resources");
        d.prefix = settings.value("prefix").toString();
        settings.endGroup();
    }

    if (groups.contains("StyleSheets")) {
        settings.beginGroup("StyleSheets");
        d.css = readFile(settings.value("document").toString());
        d.qss = readFile(settings.value("application").toString());
        settings.endGroup();
    }

    return !d.attributes.value("name").isEmpty();
}
