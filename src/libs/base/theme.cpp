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

#include "theme.h"
#include <QPalette>

Theme::Theme(QObject* parent) : QObject(parent)
{
    QPalette pal;
    d.badge = qMakePair(pal.buttonText(), pal.mid());
    d.flash = qMakePair(QBrush(qRgb(255, 64, 64)), QBrush(qRgb(255, 80, 80)));
    d.highlight = qMakePair(QBrush(qRgba(255, 64, 64, 64)), QBrush(qRgba(255, 80, 80, 64)));
    d.lowlight = qMakePair(pal.mid(), pal.alternateBase());
    d.border = pal.color(QPalette::Mid);
    d.dash = pal.color(QPalette::Mid);
}

Theme* Theme::instance()
{
    static Theme theme;
    return &theme;
}

static QBrush brushRole(const QPair<QBrush, QBrush>& brushes, Theme::Role role)
{
    return role == Theme::Foreground ? brushes.first : brushes.second;
}

static bool setBrushValue(QBrush* brush, const QBrush& value)
{
    Q_ASSERT(brush);
    if (*brush != value) {
        *brush = value;
        return true;
    }
    return false;
}

static bool setBrushRole(QPair<QBrush, QBrush>* brushes, const QBrush& brush, Theme::Role role)
{
    Q_ASSERT(brushes);
    if (role == Theme::Foreground)
        return setBrushValue(&brushes->first, brush);
    else
        return setBrushValue(&brushes->second, brush);
}

QBrush Theme::badge(Role role) const
{
    return brushRole(d.badge, role);
}

void Theme::setBadge(Role role, const QBrush& brush)
{
    if (setBrushRole(&d.badge, brush, role))
        emit changed();
}

QBrush Theme::flash(Role role) const
{
    return brushRole(d.flash, role);
}

void Theme::setFlash(Role role, const QBrush& brush)
{
    if (setBrushRole(&d.flash, brush, role))
        emit changed();
}

QBrush Theme::highlight(Role role) const
{
    return brushRole(d.highlight, role);
}

void Theme::setHighlight(Role role, const QBrush& brush)
{
    if (setBrushRole(&d.highlight, brush, role))
        emit changed();
}

QBrush Theme::lowlight(Role role) const
{
    return brushRole(d.lowlight, role);
}

void Theme::setLowlight(Role role, const QBrush& brush)
{
    if (setBrushRole(&d.lowlight, brush, role))
        emit changed();
}

QColor Theme::border() const
{
    return d.border;
}

void Theme::setBorder(const QColor& color)
{
    if (d.border != color) {
        d.border = color;
        emit changed();
    }
}

QColor Theme::dash() const
{
    return d.dash;
}

void Theme::setDash(const QColor& color)
{
    if (d.dash != color) {
        d.dash = color;
        emit changed();
    }
}
