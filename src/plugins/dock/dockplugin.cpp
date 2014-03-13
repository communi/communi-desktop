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

#include "dockplugin.h"
#include "qtdocktile.h"
#include "bufferview.h"
#include "textdocument.h"
#include <IrcConnection>
#include <QWidget>
#include <QEvent>

DockPlugin::DockPlugin(QObject* parent) : QObject(parent)
{
    d.dock = 0;
    d.window = 0;
}

void DockPlugin::initView(BufferView* view)
{
    if (!d.window && QtDockTile::isAvailable()) {
        d.window = view->window();
        d.window->installEventFilter(this);
        d.dock = new QtDockTile(d.window);
    }
}

void DockPlugin::initDocument(TextDocument* document)
{
    if (d.dock && !document->isClone())
        connect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onMessageHighlighted(IrcMessage*)));
}

void DockPlugin::cleanupDocument(TextDocument* document)
{
    if (d.dock && !document->isClone())
        disconnect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onMessageHighlighted(IrcMessage*)));
}

void DockPlugin::onMessageHighlighted(IrcMessage* message)
{
    if (d.dock && !d.window->isActiveWindow())
        d.dock->setBadge(d.dock->badge() + 1);
}

bool DockPlugin::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ActivationChange) {
        if (d.window->isActiveWindow())
            d.dock->setBadge(0);
    }
    return false;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(DockPlugin)
#endif
