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

#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <QLineEdit>

class IrcBuffer;
class IrcCommandParser;

class TextInput : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)
    Q_PROPERTY(IrcCommandParser* parser READ parser WRITE setParser NOTIFY parserChanged)

public:
    TextInput(QWidget* parent = 0);

    IrcBuffer* buffer() const;
    IrcCommandParser* parser() const;

public slots:
    void setBuffer(IrcBuffer* buffer);
    void setParser(IrcCommandParser* parser);

signals:
    void bufferChanged(IrcBuffer* buffer);
    void parserChanged(IrcCommandParser* parser);

private slots:
    void sendInput();

private:
    struct Private {
        IrcBuffer* buffer;
        IrcCommandParser* parser;
    } d;
};

#endif // TEXTINPUT_H
