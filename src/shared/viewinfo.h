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

#ifndef VIEWINFO_H
#define VIEWINFO_H

#include <QString>
#include <QMetaType>
#include <QDataStream>
#include "streamer.h"

struct ViewInfo
{
    enum Type { Server, Channel, Query };
    ViewInfo() : type(-1), active(false), expanded(true) { }
    QString name;
    QString key;
    int type; // 124
    bool active; // 125
    bool expanded; // 126
};
Q_DECLARE_METATYPE(ViewInfo);

inline QDataStream& operator<<(QDataStream& out, const ViewInfo& view)
{
    out << quint32(126); // version
    out << view.name;
    out << view.key;
    out << view.type; // 124
    out << view.active; // 125
    out << view.expanded; // 126
    return out;
}

inline QDataStream& operator>>(QDataStream& in, ViewInfo& view)
{
    quint32 version = readStreamValue<quint32>(in, 0);
    view.name = readStreamValue<QString>(in, view.name);
    view.key = readStreamValue<QString>(in, view.key);
    if (version >= 124)
        view.type = readStreamValue<uint>(in, view.type);
    if (version >= 125)
        view.active = readStreamValue<bool>(in, view.active);
    if (version >= 126)
        view.expanded = readStreamValue<bool>(in, view.expanded);
    return in;
}

typedef QList<ViewInfo> ViewInfos;
Q_DECLARE_METATYPE(ViewInfos);

#endif // VIEWINFO_H
