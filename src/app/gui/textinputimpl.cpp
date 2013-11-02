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

#include "textinputimpl.h"
#include "inputplugin.h"
#include "commandparser.h"
#include <QPluginLoader> // TODO
#include <IrcBuffer>

// TODO:
Q_IMPORT_PLUGIN(CompleterPlugin)
Q_IMPORT_PLUGIN(HistoryPlugin)

TextInputImpl::TextInputImpl(QWidget* parent) : TextInput(parent)
{
    CommandParser* parser = new CommandParser(this);
    parser->setTriggers(QStringList("/"));
    parser->setTolerant(true);
    setParser(parser);

    setAttribute(Qt::WA_MacShowFocusRect, false);

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        InputPlugin* plugin = qobject_cast<InputPlugin*>(instance);
        if (plugin)
            plugin->initialize(this);
    }

    connect(this, SIGNAL(returnPressed()), this, SLOT(sendInput()));
}

void TextInputImpl::sendInput()
{
    IrcBuffer* b = buffer();
    IrcCommandParser* p = parser();
    if (!b || !p)
        return;

    bool error = false;
    const QStringList lines = text().split(QRegExp("[\\r\\n]"), QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        if (!line.trimmed().isEmpty()) {
            if (!b->sendCommand(p->parse(line)))
                error = true;
        }
    }
    if (!error)
        clear();
}
