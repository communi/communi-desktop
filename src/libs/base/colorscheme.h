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

#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <QObject>
#include <QColor>
#include <QBrush>
#include <QPair>

#define colorScheme ColorScheme::instance()

class ColorScheme : public QObject
{
    Q_OBJECT

public:
    static ColorScheme* instance();

    enum Role { Foreground, Background };

    QBrush badge(Role role) const;
    void setBadge(Role role, const QBrush& brush);

    QBrush flash(Role role) const;
    void setFlash(Role role, const QBrush& brush);

    QBrush highlight(Role role) const;
    void setHighlight(Role role, const QBrush& brush);

    QBrush lowlight(Role role) const;
    void setLowlight(Role role, const QBrush& brush);

    QColor border() const;
    void setBorder(const QColor& color);

    QColor dash() const;
    void setDash(const QColor& color);

signals:
    void changed();

private:
    ColorScheme(QObject* parent = 0);

    struct Private {
        QPair<QBrush, QBrush> badge;
        QPair<QBrush, QBrush> flash;
        QPair<QBrush, QBrush> highlight;
        QPair<QBrush, QBrush> lowlight;
        QColor border;
        QColor dash;
    } d;
};

#endif // COLORSCHEME_H
