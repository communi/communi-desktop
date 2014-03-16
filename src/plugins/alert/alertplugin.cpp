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

#include "alert.h"
#include "alertplugin.h"
#include "textdocument.h"
#include <QDesktopServices>
#include <QApplication>
#include <IrcBuffer>
#include <QWidget>
#include <QFile>
#include <QDir>

inline void initResource() { Q_INIT_RESOURCE(alert); }

AlertPlugin::AlertPlugin(QObject* parent) : QObject(parent)
{
    d.alert = 0;
    if (Alert::isAvailable()) {
        d.alert = new Alert(this);

        QDir dataDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
        if (dataDir.exists() || dataDir.mkpath(".")) {
            QString filePath = dataDir.filePath("alert.mp3");
            if (!QFile::exists(filePath)) {
                initResource();
                QFile::copy(":/alert.mp3", filePath);
            }
            d.alert->setFilePath(filePath);
        }
    }
}

void AlertPlugin::documentAdded(TextDocument* document)
{
    if (!document->isClone())
        connect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onMessageHighlighted(IrcMessage*)));
}

void AlertPlugin::documentRemoved(TextDocument* document)
{
    if (!document->isClone())
        disconnect(document, SIGNAL(messageHighlighted(IrcMessage*)), this, SLOT(onMessageHighlighted(IrcMessage*)));
}

void AlertPlugin::onMessageHighlighted(IrcMessage* message)
{
    Q_UNUSED(message);
    if (d.alert) {
        TextDocument* document = qobject_cast<TextDocument*>(sender());
        if (!isActiveWindow() || document != currentDocument()) {
            d.alert->play();
            if (!isActiveWindow())
                QApplication::alert(window());
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(AlertPlugin)
#endif
