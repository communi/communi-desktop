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

#include "soundnotification.h"
#include "application.h"

#if defined(QT_MULTIMEDIA_LIB)
#include <QMediaPlayer>
#elif defined(QT_PHONON_LIB)
#include <MediaObject>
#endif

SoundNotification::SoundNotification(QObject* parent) : QObject(parent)
{
#if defined(QT_MULTIMEDIA_LIB)
    player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(Application::dataDir().filePath("notify.mp3")));
#elif defined(QT_PHONON_LIB)
    player = Phonon::createPlayer(Phonon::MusicCategory,
                                  Phonon::MediaSource(Application::dataDir().filePath("notify.mp3")));
    player->setParent(this);
#endif
}

bool SoundNotification::isAvailable()
{
#if defined(QT_MULTIMEDIA_LIB) || defined(QT_PHONON_LIB)
    return true;
#else
    return false;
#endif
}

void SoundNotification::play()
{
#if defined(QT_MULTIMEDIA_LIB) || defined(QT_PHONON_LIB)
        player->play();
#endif
}
