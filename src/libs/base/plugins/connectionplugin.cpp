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

#include "connectionplugin.h"
#include <IrcConnection>
#include <QApplication>
#include <QVariant>
#include <QWidget>

// TODO:
static QWidget* findCommuniMainWindow()
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

QList<IrcConnection*> ConnectionPlugin::connections() const
{
    QWidget* window = findCommuniMainWindow();
    if (window)
        return window->property("connections").value<QList<IrcConnection*> >();
    return QList<IrcConnection*>();
}

void ConnectionPlugin::addConnection(IrcConnection* connection)
{
    QWidget* window = findCommuniMainWindow();
    if (window)
        QMetaObject::invokeMethod(window, "addConnection", Q_ARG(IrcConnection*, connection));
}

void ConnectionPlugin::removeConnection(IrcConnection* connection)
{
    QWidget* window = findCommuniMainWindow();
    if (window)
        QMetaObject::invokeMethod(window, "removeConnection", Q_ARG(IrcConnection*, connection));
}
