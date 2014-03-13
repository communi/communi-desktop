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

#include "importerplugin.h"
#include "connectioninfo.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

ImporterPlugin::ImporterPlugin(QObject* parent) : QObject(parent)
{
    qRegisterMetaTypeStreamOperators<ConnectionInfo>("ConnectionInfo");
    qRegisterMetaTypeStreamOperators<ConnectionInfos>("ConnectionInfos");
}

void ImporterPlugin::windowCreated(QWidget* window)
{
    QSettings settings;
    if (!settings.value("imported", false).toBool()) {
        ConnectionInfos infos = settings.value("connections").value<ConnectionInfos>();
        foreach (const ConnectionInfo& info, infos)
            addConnection(info.toConnection(window));
        settings.setValue("imported", true);
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(ImporterPlugin)
#endif
