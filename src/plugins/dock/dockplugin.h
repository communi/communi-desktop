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

#ifndef DOCKPLUGIN_H
#define DOCKPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "documentplugin.h"
#include "windowplugin.h"

class QtDockTile;
class IrcMessage;

class DockPlugin : public QObject, public DocumentPlugin, public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(DocumentPlugin WindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")
#endif

public:
    DockPlugin(QObject* parent = 0);

    void initWindow(QWidget* window);

    void documentAdded(TextDocument* document);
    void documentRemoved(TextDocument* document);

    void windowActivated();

private slots:
    void onMessageHighlighted(IrcMessage* message);

private:
    struct Private {
        QtDockTile* dock;
    } d;
};

#endif // DOCKPLUGIN_H
