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

#ifndef ALERTPLUGIN_H
#define ALERTPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "documentplugin.h"
#include "windowplugin.h"

class Alert;
class IrcMessage;

class AlertPlugin : public QObject, public DocumentPlugin, public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(DocumentPlugin WindowPlugin)
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")

public:
    AlertPlugin(QObject* parent = 0);

    void documentAdded(TextDocument* document);
    void documentRemoved(TextDocument* document);

private slots:
    void onMessageHighlighted(IrcMessage* message);

private:
    struct Private {
        Alert* alert;
    } d;
};

#endif // ALERTPLUGIN_H
