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

#include "windowplugin.h"
#include <QApplication>
#include <QPointer>

QWidget* findCommuniMainWindow()
{
    static QPointer<QWidget> window;
    if (!window) {
        foreach (QWidget* widget, QApplication::topLevelWidgets()) {
            if (widget->inherits("QMainWindow")) {
                window = widget;
                break;
            }
        }
    }
    return window;
}

QWidget* WindowPlugin::window() const
{
    return findCommuniMainWindow();
}

bool WindowPlugin::isActiveWindow() const
{
    QWidget* window = findCommuniMainWindow();
    return window && window->isActiveWindow();
}
