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

QWidget* WindowPlugin::window() const
{
    // TODO: beautify
    return QApplication::topLevelWidgets().value(0);
}

bool WindowPlugin::isActiveWindow() const
{
    // TODO: beautify
    QWidget* window = QApplication::topLevelWidgets().value(0);
    return window && window->isActiveWindow();
}

bool WindowPlugin::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::ActivationChange) {
        QWidget* window = qobject_cast<QWidget*>(object);
        if (window) {
            if (window->isActiveWindow())
                windowActivated();
            else
                windowDeactivated();
        }
    }
    return false;
}

void WindowPlugin::filterWindow(QWidget* window)
{
    window->installEventFilter(this);
}
