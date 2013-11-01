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

#include "completer.h"
#include "textinput.h"
#include <QShortcut>

Completer::Completer(TextInput* input) : IrcCompleter(input)
{
    d.input = input;

    setParser(input->parser());
    setBuffer(input->buffer());

    connect(input, SIGNAL(bufferChanged(IrcBuffer*)), this, SLOT(setBuffer(IrcBuffer*)));

    QShortcut* shortcut = new QShortcut(Qt::Key_Tab, input);
    connect(shortcut, SIGNAL(activated()), this, SLOT(tryComplete()));
}

void Completer::tryComplete()
{
    complete(d.input->text(), d.input->cursorPosition());
}

void Completer::complete(const QString& text, int cursor)
{
    d.input->setText(text);
    d.input->setCursorPosition(cursor);
}
