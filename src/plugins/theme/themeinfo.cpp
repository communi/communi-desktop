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

QMap<int, QString> ThemeInfo::colors() const
{
    return d.colors;
}

QMap<QString, QPalette> ThemeInfo::palettes() const
{
    return d.palettes;
}

static QColor parseColorValue(const QVariant& value, const QColor& fallback = QColor())
{
    QString str = value.type() == QVariant::StringList ? value.toStringList().join(",") : value.toString();
    return StyleParser::parseColor(str, fallback);
}

static QString readFile(const QString& filePath)
{
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(file.readAll());
    return QString();
}

static void fillPalette(const QSettings& settings, QPalette& palette, QPalette::ColorGroup group)
{
    if (settings.contains("alternate-base"))
        palette.setColor(group, QPalette::AlternateBase, parseColorValue(settings.value("alternate-base"), palette.color(group, QPalette::AlternateBase)));
    if (settings.contains("background"))
        palette.setColor(group, QPalette::Background, parseColorValue(settings.value("background"), palette.color(group, QPalette::Background)));
    if (settings.contains("base"))
        palette.setColor(group, QPalette::Base, parseColorValue(settings.value("base"), palette.color(group, QPalette::Base)));
    if (settings.contains("bright-text"))
        palette.setColor(group, QPalette::BrightText, parseColorValue(settings.value("bright-text"), palette.color(group, QPalette::BrightText)));
    if (settings.contains("button"))
        palette.setColor(group, QPalette::Button, parseColorValue(settings.value("button"), palette.color(group, QPalette::Button)));
    if (settings.contains("button-text"))
        palette.setColor(group, QPalette::ButtonText, parseColorValue(settings.value("button-text"), palette.color(group, QPalette::ButtonText)));
    if (settings.contains("dark"))
        palette.setColor(group, QPalette::Dark, parseColorValue(settings.value("dark"), palette.color(group, QPalette::Dark)));
    if (settings.contains("foreground"))
        palette.setColor(group, QPalette::Foreground, parseColorValue(settings.value("foreground"), palette.color(group, QPalette::Foreground)));
    if (settings.contains("highlight"))
        palette.setColor(group, QPalette::Highlight, parseColorValue(settings.value("highlight"), palette.color(group, QPalette::Highlight)));
    if (settings.contains("highlighted-text"))
        palette.setColor(group, QPalette::HighlightedText, parseColorValue(settings.value("highlighted-text"), palette.color(group, QPalette::HighlightedText)));
    if (settings.contains("light"))
        palette.setColor(group, QPalette::Light, parseColorValue(settings.value("light"), palette.color(group, QPalette::Light)));
    if (settings.contains("link"))
        palette.setColor(group, QPalette::Link, parseColorValue(settings.value("link"), palette.color(group, QPalette::Link)));
    if (settings.contains("link-visited"))
        palette.setColor(group, QPalette::LinkVisited, parseColorValue(settings.value("link-visited"), palette.color(group, QPalette::LinkVisited)));
    if (settings.contains("mid"))
        palette.setColor(group, QPalette::Mid, parseColorValue(settings.value("mid"), palette.color(group, QPalette::Mid)));
    if (settings.contains("midlight"))
        palette.setColor(group, QPalette::Midlight, parseColorValue(settings.value("midlight"), palette.color(group, QPalette::Midlight)));
    if (settings.contains("shadow"))
        palette.setColor(group, QPalette::Shadow, parseColorValue(settings.value("shadow"), palette.color(group, QPalette::Shadow)));
    if (settings.contains("text"))
        palette.setColor(group, QPalette::Text, parseColorValue(settings.value("text"), palette.color(group, QPalette::Text)));
    if (settings.contains("window"))
        palette.setColor(group, QPalette::Window, parseColorValue(settings.value("window"), palette.color(group, QPalette::Window)));
    if (settings.contains("window-text"))
        palette.setColor(group, QPalette::WindowText, parseColorValue(settings.value("window-text"), palette.color(group, QPalette::WindowText)));
}

static QPalette readPalette(QSettings& settings)
{
    QPalette palette;
    fillPalette(settings, palette, QPalette::Normal);
    QStringList groups = settings.childGroups();
    if (groups.contains("normal")) {
        settings.beginGroup("normal");
        fillPalette(settings, palette, QPalette::Normal);
        settings.endGroup();
    }
    if (groups.contains("disabled")) {
        settings.beginGroup("disabled");
        fillPalette(settings, palette, QPalette::Disabled);
        settings.endGroup();
    }
    if (groups.contains("inactive")) {
        settings.beginGroup("inactive");
        fillPalette(settings, palette, QPalette::Inactive);
        settings.endGroup();
    }
    if (groups.contains("active")) {
        settings.beginGroup("active");
        fillPalette(settings, palette, QPalette::Active);
        settings.endGroup();
    }
    return palette;
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

    if (groups.contains("QPalette")) {
        settings.beginGroup("QPalette");
        foreach (const QString& group, settings.childGroups()) {
            settings.beginGroup(group);
            d.palettes.insert(group, readPalette(settings));
            settings.endGroup();
        }
        settings.endGroup();
    }

    if (groups.contains("IrcPalette")) {
        settings.beginGroup("IrcPalette");
        const QStringList colorNames = QStringList() << "white" << "black" << "blue" << "green"
                                                     << "red" << "brown" << "purple" << "orange"
                                                     << "yellow" << "lightgreen" << "cyan" << "lightcyan"
                                                     << "lightblue" << "pink" << "gray" << "lightgray";
        for (int i = 0; i < colorNames.count(); ++i) {
            const QColor color = parseColorValue(settings.value(colorNames.at(i)).toString(), colorNames.at(i));
            if (color.isValid())
                d.colors.insert(i, color.name());
        }
        settings.endGroup();
    }

    return !d.attributes.value("name").isEmpty();
}
