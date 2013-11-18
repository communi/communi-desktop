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

#ifndef THEME_H
#define THEME_H

#include <QMap>
#include <QString>
#include <QPalette>

class Theme
{
public:
    bool load(const QString& filePath);

    QString attribute(const QString& key) const;

    QString prefix() const;

    QString docStyleSheet() const;
    QString appStyleSheet() const;

    QPalette appPalette() const;
    QMap<int, QString> ircPalette() const;

private:
    struct Private {
        QString prefix;
        QString css;
        QString qss;
        QPalette palette;
        QMap<int, QString> colors;
        QMap<QString, QString> attributes;
    } d;
};

#endif // THEME_H
