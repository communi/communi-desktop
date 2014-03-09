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
#include "textdocumentplugin.h"

class IrcCommand;
class IrcMessage;
class CommandVerifier;

class VerifierPlugin : public QObject, public ConnectionPlugin, public TextDocumentPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin TextDocumentPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TextDocumentPlugin")
#endif

public:
    VerifierPlugin(QObject* parent = 0);

    void initConnection(IrcConnection* connection);
    void initDocument(TextDocument* document);

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
