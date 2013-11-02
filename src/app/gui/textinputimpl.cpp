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
#include <IrcBufferModel>
#include <QPluginLoader> // TODO
#include <IrcBuffer>

// TODO:
Q_IMPORT_PLUGIN(CompleterPlugin)
Q_IMPORT_PLUGIN(HistoryPlugin)

TextInputImpl::TextInputImpl(QWidget* parent) : TextInput(parent)
{
    d.buffer = 0;

    d.parser = new CommandParser(this);
    d.parser->setTriggers(QStringList("/"));
    d.parser->setTolerant(true);

    setAttribute(Qt::WA_MacShowFocusRect, false);

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        InputPlugin* plugin = qobject_cast<InputPlugin*>(instance);
        if (plugin)
            plugin->initialize(this);
    }

    connect(this, SIGNAL(returnPressed()), this, SLOT(sendInput()));
}

IrcBuffer* TextInputImpl::buffer() const
{
    return d.buffer;
}

IrcCommandParser* TextInputImpl::parser() const
{
    return d.parser;
}

void TextInputImpl::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            IrcBufferModel* model = d.buffer->model();
            disconnect(model, SIGNAL(channelsChanged(QStringList)), d.parser, SLOT(setChannels(QStringList)));
            disconnect(buffer, SIGNAL(titleChanged(QString)), d.parser, SLOT(setTarget(QString)));
        }

        if (buffer) {
            IrcBufferModel* model = buffer->model();
            connect(model, SIGNAL(channelsChanged(QStringList)), d.parser, SLOT(setChannels(QStringList)));
            connect(buffer, SIGNAL(titleChanged(QString)), d.parser, SLOT(setTarget(QString)));

            d.parser->setTarget(buffer->title());
            d.parser->setChannels(buffer->model()->channels());
        } else {
            d.parser->reset();
        }

        d.buffer = buffer;
        emit bufferChanged(buffer);
    }
}

void TextInputImpl::sendInput()
{
    if (!d.buffer)
        return;

    bool error = false;
    const QStringList lines = text().split(QRegExp("[\\r\\n]"), QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        if (!line.trimmed().isEmpty()) {
            if (!d.buffer->sendCommand(d.parser->parse(line)))
                error = true;
        }
    }
    if (!error)
        clear();
}
