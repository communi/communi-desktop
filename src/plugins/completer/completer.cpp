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
    connect(this, SIGNAL(completed(QString,int)), this, SLOT(doComplete(QString,int)));
}

void Completer::tryComplete()
{
    if (d.input->hasFocus())
        complete(d.input->text(), d.input->cursorPosition());
}

void Completer::doComplete(const QString& text, int cursor)
{
    d.input->setText(text);
    d.input->setCursorPosition(cursor);
}
