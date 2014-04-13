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

#ifndef ZNCPLUGIN_H
#define ZNCPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include <QMultiHash>
#include "connectionplugin.h"
#include "documentplugin.h"

class IrcBuffer;
class IrcConnection;

class ZncPlugin : public QObject, public ConnectionPlugin, public DocumentPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin DocumentPlugin)
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")

public:
    ZncPlugin(QObject* parent = 0);

    void connectionAdded(IrcConnection* connection);
    void documentAdded(TextDocument* document);
    void documentRemoved(TextDocument* document);

private slots:
    void onPlaybackBegin(IrcBuffer* buffer);
    void onPlaybackEnd(IrcBuffer* buffer);

private:
    struct Private {
        QMultiHash<IrcBuffer*, TextDocument*> documents;
    } d;
};

#endif // ZNCPLUGIN_H
