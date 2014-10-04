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

#include "treespinner.h"
#include <QHash>

TreeSpinner::TreeSpinner(QWidget* parent) : QFrame(parent)
{
    setVisible(false);
}

TreeSpinner* TreeSpinner::instance(QWidget* parent)
{
    static QHash<QWidget*, TreeSpinner*> spinners;
    QWidget* window = parent ? parent->window() : 0;
    TreeSpinner* spinner = spinners.value(window);
    if (!spinner) {
        spinner = new TreeSpinner(window);
        spinners.insert(window, spinner);
    }
    return spinner;
}
