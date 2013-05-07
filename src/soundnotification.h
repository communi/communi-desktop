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

#ifndef SOUNDNOTIFICATION_H
#define SOUNDNOTIFICATION_H

#include <QObject>

#if defined(QT_MULTIMEDIA_LIB)
class QMediaPlayer;
#elif defined(QT_PHONON_LIB)
namespace Phonon { class MediaObject; }
#endif

class SoundNotification : public QObject
{
    Q_OBJECT

public:
    explicit SoundNotification(QObject* parent = 0);

    static bool isAvailable();

public slots:
    void play();

private:
#if defined(QT_MULTIMEDIA_LIB)
    QMediaPlayer* player;
#elif defined(QT_PHONON_LIB)
    Phonon::MediaObject* player;
#endif
};

#endif // SOUNDNOTIFICATION_H
