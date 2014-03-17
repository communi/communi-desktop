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

#ifndef VERIFIERPLUGIN_H
#define VERIFIERPLUGIN_H

#include <QHash>
#include <QtPlugin>
#include <QMultiHash>
#include "connectionplugin.h"
#include "documentplugin.h"

class IrcCommand;
class IrcMessage;
class CommandVerifier;

class VerifierPlugin : public QObject, public ConnectionPlugin, public DocumentPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin DocumentPlugin)
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")

public:
    VerifierPlugin(QObject* parent = 0);

    void connectionAdded(IrcConnection* connection);
    void documentAdded(TextDocument* document);

private slots:
    void onCommandVerified(int id);
    void onMessageReceived(IrcMessage* message);

private:
    struct Private {
        QMultiHash<int, TextDocument*> documents;
        QHash<IrcConnection*, CommandVerifier*> verifiers;
    } d;
};

#endif // VERIFIERPLUGIN_H
