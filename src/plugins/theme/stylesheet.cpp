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

#include "stylesheet.h"
#include <QApplication>
#include <IrcPalette>
#include <QPalette>
#include <QColor>
#include <QFile>

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

static QColor parseColorValue(const QString& str)
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
    return color;
}

static QColor parseColor(const QString& css, const QString& name)
{
    QRegExp rx(QString("%1\\s*:\\s+(?:\")?([^;\"\\r\\n]+)").arg(name));
    int index = rx.indexIn(css);
    if (index != -1)
        return parseColorValue(rx.cap(1));
    return QColor();
}

bool StyleSheet::load(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QIODevice::Text))
        return false;

    QString css = QString::fromUtf8(file.readAll());
//    if (d.css != css) {
//        d.css = css;
//        emit styleSheetChanged(css);
//    }

    if (!css.isEmpty()) {
        int index = css.indexOf("IrcPalette");
        if (index != -1) {
            int from = css.indexOf("{", index);
            if (from != -1) {
                int to = css.indexOf("}", from);
                if (to != -1) {
                    const QString block = css.mid(from + 1, to - from - 1);

                    const QStringList colorNames = QStringList() << "white" << "black" << "blue" << "green"
                                                                 << "red" << "brown" << "purple" << "orange"
                                                                 << "yellow" << "lightgreen" << "cyan" << "lightcyan"
                                                                 << "lightblue" << "pink" << "gray" << "lightgray";

//                    IrcPalette palette;
//                    for (int i = 0; i < colorNames.count(); ++i) {
//                        QColor color = parseColor(block, colorNames.at(i));
//                        if (color.isValid())
//                            palette.setColorName(i, color.name());
//                    }

//                    if (d.palette != palette) {
//                        d.palette = palette;
//                        emit paletteChanged(d.palette);
//                    }
                    return true;
                }
            }
            // parsing IrcPalette css failed
            return false;
        }
    }
    // empty css is ok!
    return true;
}
