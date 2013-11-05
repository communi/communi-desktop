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

#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "windowplugin.h"

class SoundNotification;

class SoundPlugin : public QObject, public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(WindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.github.communi.WindowPlugin")
#endif

public:
    SoundPlugin(QObject* parent = 0);

    void initialize(QWidget* window);

private:
    struct Private {
        QWidget* window;
        SoundNotification* sound;
    } d;
};

#endif // SOUNDPLUGIN_H
