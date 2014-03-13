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

#include "soundplugin.h"
#include "textdocument.h"
#include "soundnotification.h"
#include <QDesktopServices>
#include <QApplication>
#include <IrcBuffer>
#include <QWidget>
#include <QFile>
#include <QDir>

inline void initResource() { Q_INIT_RESOURCE(sound); }

SoundPlugin::SoundPlugin(QObject* parent) : QObject(parent)
{
    d.sound = 0;
    if (SoundNotification::isAvailable()) {
        d.sound = new SoundNotification(this);

        QDir dataDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
        if (dataDir.exists() || dataDir.mkpath(".")) {
            QString filePath = dataDir.filePath("sound.mp3");
            if (!QFile::exists(filePath)) {
                initResource();
                QFile::copy(":/sound.mp3", filePath);
            }
            d.sound->setFilePath(filePath);
        }
    }
}

void SoundPlugin::initDocument(TextDocument* document)
{
    if (!document->isClone())
        connect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onMessageHighlighted(IrcMessage*)));
}

void SoundPlugin::cleanupDocument(TextDocument* document)
{
    if (!document->isClone())
        disconnect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onMessageHighlighted(IrcMessage*)));
}

void SoundPlugin::onMessageHighlighted(IrcMessage* message)
{
    Q_UNUSED(message);
    if (d.sound) {
        TextDocument* document = qobject_cast<TextDocument*>(sender());
        QWidget* window = QApplication::topLevelWidgets().value(0);
        if ((window && !window->isActiveWindow()) || document != currentDocument()) {
            d.sound->play();
            if (window && !window->isActiveWindow())
                QApplication::alert(window);
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(SoundPlugin)
#endif
