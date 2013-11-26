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

static QColor parsePaletteColor(const QString& str)
{
    if (str == "alternate-base")
        return qApp->palette().color(QPalette::AlternateBase);
    else if (str == "base")
        return qApp->palette().color(QPalette::Base);
    else if (str == "bright-text")
        return qApp->palette().color(QPalette::BrightText);
    else if (str == "button")
        return qApp->palette().color(QPalette::Button);
    else if (str == "button-text")
        return qApp->palette().color(QPalette::ButtonText);
    else if (str == "dark")
        return qApp->palette().color(QPalette::Dark);
    else if (str == "highlight")
        return qApp->palette().color(QPalette::Highlight);
    else if (str == "highlighted-text")
        return qApp->palette().color(QPalette::HighlightedText);
    else if (str == "light")
        return qApp->palette().color(QPalette::Light);
    else if (str == "link")
        return qApp->palette().color(QPalette::Link);
    else if (str == "link-visited")
        return qApp->palette().color(QPalette::LinkVisited);
    else if (str == "mid")
        return qApp->palette().color(QPalette::Mid);
    else if (str == "midlight")
        return qApp->palette().color(QPalette::Midlight);
    else if (str == "shadow")
        return qApp->palette().color(QPalette::Shadow);
    else if (str == "text")
        return qApp->palette().color(QPalette::Text);
    else if (str == "window")
        return qApp->palette().color(QPalette::Window);
    else if (str == "window-text")
        return qApp->palette().color(QPalette::WindowText);
    return QColor();
}

static QColor parseRgbColor(const QString& str)
{
    QStringList rgb = str.split(",", QString::SkipEmptyParts);
    if (rgb.count() == 3)
        return qRgb(rgb.first().toInt(), rgb.at(1).toInt(), rgb.last().toInt());
    return QColor();
}

static QColor parseRgbaColor(const QString& str)
{
    QStringList rgba = str.split(",", QString::SkipEmptyParts);
    if (rgba.count() == 4)
        return qRgba(rgba.first().toInt(), rgba.at(1).toInt(), rgba.at(2).toInt(), rgba.last().toInt());
    return QColor();
}

static QColor parseColorValue(const QString& str, const QColor& defaultColor = QColor())
{
    QColor color;
    if (str.startsWith("palette(") && str.endsWith(")"))
        color = parsePaletteColor(str.mid(8, str.length() - 9));
    else if (str.startsWith("rgb(") && str.endsWith(")"))
        color = parseRgbColor(str.mid(4, str.length() - 5));
    else if (str.startsWith("rgba(") && str.endsWith(")"))
        color = parseRgbaColor(str.mid(5, str.length() - 6));
    else if (QColor::isValidColor(str))
        color = QColor(str);
    return color.isValid() ? color : defaultColor;
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
    palette.setColor(group, QPalette::AlternateBase, parseColorValue(settings.value("alternate-base").toString(), palette.color(group, QPalette::AlternateBase)));
    palette.setColor(group, QPalette::Base, parseColorValue(settings.value("base").toString(), palette.color(group, QPalette::Base)));
    palette.setColor(group, QPalette::BrightText, parseColorValue(settings.value("bright-text").toString(), palette.color(group, QPalette::BrightText)));
    palette.setColor(group, QPalette::Button, parseColorValue(settings.value("button").toString(), palette.color(group, QPalette::Button)));
    palette.setColor(group, QPalette::ButtonText, parseColorValue(settings.value("button-text").toString(), palette.color(group, QPalette::ButtonText)));
    palette.setColor(group, QPalette::Dark, parseColorValue(settings.value("dark").toString(), palette.color(group, QPalette::Dark)));
    palette.setColor(group, QPalette::Highlight, parseColorValue(settings.value("highlight").toString(), palette.color(group, QPalette::Highlight)));
    palette.setColor(group, QPalette::HighlightedText, parseColorValue(settings.value("highlighted-text").toString(), palette.color(group, QPalette::HighlightedText)));
    palette.setColor(group, QPalette::Light, parseColorValue(settings.value("light").toString(), palette.color(group, QPalette::Light)));
    palette.setColor(group, QPalette::Link, parseColorValue(settings.value("link").toString(), palette.color(group, QPalette::Link)));
    palette.setColor(group, QPalette::LinkVisited, parseColorValue(settings.value("link-visited").toString(), palette.color(group, QPalette::LinkVisited)));
    palette.setColor(group, QPalette::Mid, parseColorValue(settings.value("mid").toString(), palette.color(group, QPalette::Mid)));
    palette.setColor(group, QPalette::Midlight, parseColorValue(settings.value("midlight").toString(), palette.color(group, QPalette::Midlight)));
    palette.setColor(group, QPalette::Shadow, parseColorValue(settings.value("shadow").toString(), palette.color(group, QPalette::Shadow)));
    palette.setColor(group, QPalette::Text, parseColorValue(settings.value("text").toString(), palette.color(group, QPalette::Text)));
    palette.setColor(group, QPalette::Window, parseColorValue(settings.value("window").toString(), palette.color(group, QPalette::Window)));
    palette.setColor(group, QPalette::WindowText, parseColorValue(settings.value("window-text").toString(), palette.color(group, QPalette::WindowText)));
}

static QPalette readPalette(QSettings& settings)
{
    QPalette palette;
    fillPalette(settings, palette, QPalette::Normal);
    settings.beginGroup("normal");
    fillPalette(settings, palette, QPalette::Normal);
    settings.endGroup();
    settings.beginGroup("disabled");
    fillPalette(settings, palette, QPalette::Disabled);
    settings.endGroup();
    settings.beginGroup("inactive");
    fillPalette(settings, palette, QPalette::Inactive);
    settings.endGroup();
    settings.beginGroup("active");
    fillPalette(settings, palette, QPalette::Active);
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
