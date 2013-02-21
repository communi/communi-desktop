/*
* Copyright (C) 2008-2013 Communi authors
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMap>
#include <QHash>
#include <QVariant>
#include <QMetaType>
#include "streamer.h"

struct Settings {
    Settings();
    operator QVariant() const;
    bool operator==(const Settings& other) const;
    bool operator!=(const Settings& other) const;

    enum MessageType {
        Joins,
        Parts,
        Nicks,
        Modes,
        Kicks,
        Quits,
        Topics
    };

    enum ColorType {
        Background,
        Message,
        Event,
        Notice,
        Action,
        Highlight,
        TimeStamp,
        Link
    };

    enum ShortcutType {
        PreviousView = 0, // was NavigateUp
        NextView = 1, // was NavigateDown
        CollapseView = 2, // was NavigateLeft
        ExpandView = 3, // was NavigateRight
        PreviousUnreadView = 4, // was NextUnreadUp
        NextUnreadView = 5, // was NextUnreadDown
        // 6 was NextUnreadLeft
        // 7 was NextUnreadRight
        MostActiveView = 8,
        SearchView = 9
    };

    QHash<int, bool> messages;
    QHash<int, bool> highlights;
    QHash<int, QString> colors;
    QHash<int, QString> shortcuts;
    QString language;
    QString font;
    int maxBlockCount;
    bool timeStamp;
    QString layout; // deprecated
    bool stripNicks;
    QString timeStampFormat;
    QMap<QString, QString> aliases;
};
Q_DECLARE_METATYPE(Settings);

inline QDataStream& operator<<(QDataStream& out, const Settings& settings)
{
    out << quint32(132); // version
    out << settings.messages;
    out << settings.highlights;
    out << settings.language;
    out << settings.font;
    out << settings.colors;
    out << settings.shortcuts;
    out << settings.maxBlockCount;
    out << settings.timeStamp;
    out << settings.layout; // deprecated
    out << settings.stripNicks;
    out << settings.timeStampFormat;
    out << settings.aliases;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, Settings& settings)
{
    quint32 version = readStreamValue<quint32>(in, 0);
    settings.messages = readStreamValue< QHash<int, bool> >(in, settings.messages);
    settings.highlights = readStreamValue< QHash<int, bool> >(in, settings.highlights);
    settings.language = readStreamValue<QString>(in, settings.language);
    settings.font = readStreamValue<QString>(in, settings.font);
    settings.colors = readStreamValue< QHash<int, QString> >(in, settings.colors);
    settings.shortcuts = readStreamValue< QHash<int, QString> >(in, settings.shortcuts);
    settings.maxBlockCount = readStreamValue<int>(in, settings.maxBlockCount);
    settings.timeStamp = readStreamValue<bool>(in, settings.timeStamp);
    if (version >= 126) // deprecated
        settings.layout = readStreamValue<QString>(in, settings.layout);
    if (version >= 129)
        settings.stripNicks = readStreamValue<bool>(in, settings.stripNicks);
    if (version >= 130)
        settings.timeStampFormat = readStreamValue<QString>(in, settings.timeStampFormat);
    if (version >= 131)
        settings.aliases = readStreamValue< QMap<QString, QString> >(in, settings.aliases);

    Settings defaults; // default values
    if (version < 125)
        settings.colors[Settings::TimeStamp] = defaults.colors.value(Settings::TimeStamp);
    if (version < 127) {
        settings.shortcuts[Settings::NextUnreadView] = defaults.shortcuts.value(Settings::NextUnreadView);
        settings.shortcuts[Settings::PreviousUnreadView] = defaults.shortcuts.value(Settings::PreviousUnreadView);
    }
    if (version < 128)
        settings.colors[Settings::Link] = defaults.colors.value(Settings::Link);
    if (version < 132) {
        settings.shortcuts[Settings::MostActiveView] = defaults.shortcuts.value(Settings::MostActiveView);
        settings.shortcuts[Settings::SearchView] = defaults.shortcuts.value(Settings::SearchView);
    }
    return in;
}

#endif // SETTINGS_H
