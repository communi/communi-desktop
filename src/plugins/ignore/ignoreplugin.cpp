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

#include "ignoreplugin.h"
#include "ignoremanager.h"

IgnorePlugin::IgnorePlugin(QObject* parent) : QObject(parent)
{
    // TODO:
    // parser->addCommand(IrcCommand::Custom, "IGNORE (<mask>)");
    // parser->addCommand(IrcCommand::Custom, "UNIGNORE <mask>");

//    if (command == "IGNORE") {
//        MessageFormatter::Options options;
//        options.timeStampFormat = d.timeStampFormat;
//        options.timeStamp = QDateTime::currentDateTime();
//        QString ignore  = params.value(0);
//        if (ignore.isEmpty()) {
//            const QStringList ignores = IgnoreManager::instance()->ignores();
//            if (!ignores.isEmpty()) {
//                foreach (const QString& ignore, ignores)
//                    d.textBrowser->append(MessageFormatter::formatLine("! " + ignore, options));
//            } else {
//                d.textBrowser->append(MessageFormatter::formatLine("! no ignores", options));
//            }
//        } else {
//            QString mask = IgnoreManager::instance()->addIgnore(ignore);
//            d.textBrowser->append(MessageFormatter::formatLine("! ignored: " + mask));
//        }
//    } else if (command == "UNIGNORE") {
//        QString mask = IgnoreManager::instance()->removeIgnore(params.value(0));
//        d.textBrowser->append(MessageFormatter::formatLine("! unignored: " + mask));
//    }
}

void IgnorePlugin::initConnection(IrcConnection* connection)
{
    IgnoreManager::instance()->addConnection(connection);
}

void IgnorePlugin::cleanupConnection(IrcConnection* connection)
{
    IgnoreManager::instance()->removeConnection(connection);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(IgnorePlugin)
#endif
