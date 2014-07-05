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

#include "zncplugin.h"
#include "zncmanager.h"
#include "textdocument.h"
#include <IrcConnection>
#include <IrcBufferModel>
#include <IrcBuffer>

ZncPlugin::ZncPlugin(QObject* parent) : QObject(parent)
{
}

void ZncPlugin::connectionAdded(IrcConnection* connection)
{
    ZncManager* manager = new ZncManager(connection);
    manager->setModel(connection->findChild<IrcBufferModel*>());
// TODO:
//    connect(manager, SIGNAL(playbackBegin(IrcBuffer*)), this, SLOT(onPlaybackBegin(IrcBuffer*)));
//    connect(manager, SIGNAL(playbackEnd(IrcBuffer*)), this, SLOT(onPlaybackEnd(IrcBuffer*)));
}

void ZncPlugin::documentAdded(TextDocument* document)
{
    d.documents.insert(document->buffer(), document);
}

void ZncPlugin::documentRemoved(TextDocument* document)
{
    d.documents.remove(document->buffer(), document);
}

//void ZncPlugin::onPlaybackBegin(IrcBuffer* buffer)
//{
//    foreach (TextDocument* doc, d.documents.values(buffer))
//        doc->beginLowlight();
//}

//void ZncPlugin::onPlaybackEnd(IrcBuffer* buffer)
//{
//    foreach (TextDocument* doc, d.documents.values(buffer))
//        doc->endLowlight();
//}
