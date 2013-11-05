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

#include "soundplugin.h"
#include "soundnotification.h"

SoundPlugin::SoundPlugin(QObject* parent) : QObject(parent)
{
    d.sound = 0;
    d.window = 0;
}

void SoundPlugin::initialize(QWidget* window)
{
    d.window = window;

    if (SoundNotification::isAvailable()) {
        d.sound = new SoundNotification(this);

        /* TODO:
        QDir dataDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
        if (dataDir.exists() || dataDir.mkpath(".")) {
            QString filePath = dataDir.filePath("notify.mp3");
            if (!QFile::exists(filePath))
                QFile::copy(":/notify.mp3", filePath);
            d.sound->setFilePath(filePath);
        }
        */
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(SoundPlugin)
#endif
