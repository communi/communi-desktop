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

#ifndef TEXTINPUTIMPL_H
#define TEXTINPUTIMPL_H

#include "textinput.h"

class IrcBuffer;
class IrcCommandParser;

class TextInputImpl : public TextInput
{
    Q_OBJECT

public:
    TextInputImpl(QWidget* parent = 0);

    IrcBuffer* buffer() const;
    IrcCommandParser* parser() const;

public slots:
    void setBuffer(IrcBuffer* buffer);

signals:
    void bufferChanged(IrcBuffer* buffer);

private slots:
    void sendInput();

private:
    struct Private {
        IrcBuffer* buffer;
        IrcCommandParser* parser;
    } d;
};

#endif // TEXTINPUTIMPL_H
