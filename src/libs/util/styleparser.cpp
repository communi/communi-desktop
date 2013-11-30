/*
* Copyright (C) 2008-2013 The Communi Project
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "styleparser.h"
#include <QPalette>

static QColor parsePaletteColor(const QString& str)
{
    if (str == "alternate-base")
        return QPalette().color(QPalette::AlternateBase);
    else if (str == "background")
        return QPalette().color(QPalette::Background);
    else if (str == "base")
        return QPalette().color(QPalette::Base);
    else if (str == "bright-text")
        return QPalette().color(QPalette::BrightText);
    else if (str == "button")
        return QPalette().color(QPalette::Button);
    else if (str == "button-text")
        return QPalette().color(QPalette::ButtonText);
    else if (str == "dark")
        return QPalette().color(QPalette::Dark);
    else if (str == "foreground")
        return QPalette().color(QPalette::Foreground);
    else if (str == "highlight")
        return QPalette().color(QPalette::Highlight);
    else if (str == "highlighted-text")
        return QPalette().color(QPalette::HighlightedText);
    else if (str == "light")
        return QPalette().color(QPalette::Light);
    else if (str == "link")
        return QPalette().color(QPalette::Link);
    else if (str == "link-visited")
        return QPalette().color(QPalette::LinkVisited);
    else if (str == "mid")
        return QPalette().color(QPalette::Mid);
    else if (str == "midlight")
        return QPalette().color(QPalette::Midlight);
    else if (str == "shadow")
        return QPalette().color(QPalette::Shadow);
    else if (str == "text")
        return QPalette().color(QPalette::Text);
    else if (str == "window")
        return QPalette().color(QPalette::Window);
    else if (str == "window-text")
        return QPalette().color(QPalette::WindowText);
    return QColor();
}

static QColor parseRgbColor(const QString& str)
{
    QStringList rgb = str.split(",", QString::SkipEmptyParts);
    if (rgb.count() == 3)
        return QColor::fromRgb(rgb.first().toInt(), rgb.at(1).toInt(), rgb.last().toInt());
    return QColor();
}

static QColor parseRgbaColor(const QString& str)
{
    QStringList rgba = str.split(",", QString::SkipEmptyParts);
    if (rgba.count() == 4)
        return QColor::fromRgb(rgba.first().toInt(), rgba.at(1).toInt(), rgba.at(2).toInt(), rgba.last().toInt());
    return QColor();
}

static QColor parseHslColor(const QString& str)
{
    QStringList hsl = str.split(",", QString::SkipEmptyParts);
    if (hsl.count() == 3)
        return QColor::fromHsl(hsl.first().toInt(), hsl.at(1).toInt(), hsl.last().toInt());
    return QColor();
}

static QColor parseHslaColor(const QString& str)
{
    QStringList hsla = str.split(",", QString::SkipEmptyParts);
    if (hsla.count() == 4)
        return QColor::fromHsl(hsla.first().toInt(), hsla.at(1).toInt(), hsla.at(2).toInt(), hsla.last().toInt());
    return QColor();
}

static QColor parseColorValue(const QString& str, const QColor& fallback)
{
    QColor color;
    int idx = str.indexOf(")");
    if (str.startsWith("palette("))
        color = parsePaletteColor(str.mid(8, idx - 8));
    else if (str.startsWith("rgb("))
        color = parseRgbColor(str.mid(4, idx - 4));
    else if (str.startsWith("rgba("))
        color = parseRgbaColor(str.mid(5, idx - 5));
    else if (str.startsWith("hsl("))
        color = parseHslColor(str.mid(4, idx - 4));
    else if (str.startsWith("hsla("))
        color = parseHslaColor(str.mid(5, idx - 5));
    else if (QColor::isValidColor(str))
        color = QColor(str);

    QString ext = str.mid(idx + 1);
    idx = ext.indexOf(")");
    if (ext.startsWith(".darker("))
        color = color.darker(ext.mid(8, idx - 8).toInt());
    else if (ext.startsWith(".lighter("))
        color = color.lighter(ext.mid(9, idx - 9).toInt());

    return color.isValid() ? color : fallback;
}

static QString parseBlock(const QString& css, const QString& element)
{
    int index = css.indexOf(element);
    if (index != -1) {
        int from = css.indexOf("{", index);
        if (from != -1) {
            int to = css.indexOf("}", from);
            if (to != -1)
                return css.mid(from + 1, to - from - 1).trimmed();
        }
    }
    return QString();
}

static QString parseValue(const QString& block, const QString& rule)
{
    int index = block.indexOf(rule);
    if (index != -1) {
        int from = block.indexOf(":", index);
        if (from != -1) {
            int to = block.indexOf(";", from);
            if (to == -1)
                to = block.indexOf("}", from);
            if (to != -1)
                return block.mid(from + 1, to - from - 1).trimmed();
        }
    }
    return QString();
}

QColor StyleParser::parseColor(const QString& css, const QString& selector, const QColor& fallback)
{
    QString block = parseBlock(css, selector);
    if (!block.isEmpty()) {
        QString value = parseValue(block, "color");
        if (!value.isEmpty())
            return parseColorValue(value, fallback);
    }
    return fallback;
}

QColor StyleParser::parseBackground(const QString& css, const QString& selector, const QColor& fallback)
{
    QString block = parseBlock(css, selector);
    if (!block.isEmpty()) {
        QString value = parseValue(block, "background-color");
        if (!value.isEmpty())
            return parseColorValue(value, fallback);
    }
    return fallback;
}
