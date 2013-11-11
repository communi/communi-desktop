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

#ifndef SORTER_H
#define SORTER_H

#include "treewidget.h"
#include <QStringList>
#include <QHash>

typedef QHash<QString, QStringList> QHashStringList;

class Sorter
{
public:
    static bool isEnabled();
    static void setEnabled(TreeWidget* tree, bool enabled);

    static void init(TreeWidget* tree);

    static void save();
    static void restore();

    static bool sort(const TreeItem* one, const TreeItem* another);

private:
    static bool enabled;
    static QStringList parents;
    static QHashStringList children;
};
#endif // SORTER_H
