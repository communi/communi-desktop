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

#include "textinput.h"
#include "inputplugin.h"
#include "commandparser.h"
#include <QStringListModel>
#include <IrcBufferModel>
#include <IrcConnection>
#include <QPluginLoader> // TODO
#include <IrcCompleter>
#include <IrcBuffer>
#include <QShortcut>

// TODO:
Q_IMPORT_PLUGIN(HistoryPlugin)

TextInput::TextInput(QWidget* parent) : QLineEdit(parent)
{
    d.buffer = 0;

    d.parser = new CommandParser(this);
    d.parser->setTriggers(QStringList("/"));
    d.parser->setTolerant(true);

    d.completer = new IrcCompleter(this);
    connect(d.completer, SIGNAL(completed(QString,int)), this, SLOT(complete(QString,int)));
    d.completer->setParser(d.parser);

    setAttribute(Qt::WA_MacShowFocusRect, false);

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        InputPlugin* plugin = qobject_cast<InputPlugin*>(instance);
        if (plugin)
            plugin->initialize(this);
    }

    QShortcut* shortcut = new QShortcut(Qt::Key_Tab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(tryComplete()));

    connect(this, SIGNAL(returnPressed()), this, SLOT(sendInput()));
}

IrcBuffer* TextInput::buffer() const
{
    return d.buffer;
}

void TextInput::setBuffer(IrcBuffer* buffer)
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
            connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup(IrcBuffer*)), Qt::UniqueConnection);

            d.parser->setTarget(buffer->title());
            d.parser->setChannels(buffer->model()->channels());
            d.completer->setBuffer(buffer);
        } else {
            d.parser->reset();
            d.completer->setBuffer(0);
        }

        d.buffer = buffer;
        emit bufferChanged(buffer);
    }
}

void TextInput::sendInput()
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

void TextInput::cleanup(IrcBuffer* buffer)
{
    d.histories.remove(buffer);
}

void TextInput::tryComplete()
{
    d.completer->complete(text(), cursorPosition());
}

void TextInput::complete(const QString& text, int cursor)
{
    setText(text);
    setCursorPosition(cursor);
}
