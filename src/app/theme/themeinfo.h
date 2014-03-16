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

#ifndef THEMEINFO_H
#define THEMEINFO_H

#include <QString>

class ThemeInfo
{
public:
    bool isValid() const;
    bool load(const QString& filePath);

    QString name() const;
    QString author() const;
    QString version() const;
    QString description() const;
    QString style() const;

private:
    struct Private {
        QString name;
        QString author;
        QString version;
        QString description;
        QString style;
    } d;
};

#endif // THEMEINFO_H
