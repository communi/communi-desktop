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

#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "documentplugin.h"

class IrcMessage;
class SoundNotification;

class SoundPlugin : public QObject, public DocumentPlugin
{
    Q_OBJECT
    Q_INTERFACES(DocumentPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")
#endif

public:
    SoundPlugin(QObject* parent = 0);

    void initDocument(TextDocument* document);
    void cleanupDocument(TextDocument* document);

private slots:
    void onMessageHighlighted(IrcMessage* message);

private:
    struct Private {
        SoundNotification* sound;
    } d;
};

#endif // SOUNDPLUGIN_H
