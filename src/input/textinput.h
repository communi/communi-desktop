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
class IrcCompleter;
class CommandParser;

class TextInput : public QLineEdit
{
    Q_OBJECT

public:
    TextInput(QWidget* parent = 0);

    IrcBuffer* buffer() const;

public slots:
    void setBuffer(IrcBuffer* buffer);

signals:
    void bufferChanged(IrcBuffer* buffer);

private slots:
    void sendInput();
    void cleanup(IrcBuffer* buffer);

    void tryComplete();
    void complete(const QString& text, int cursor);

private:
    struct Private {
        IrcBuffer* buffer;
        CommandParser* parser;
        IrcCompleter* completer;
        QHash<IrcBuffer*, QStringList> histories;
    } d;
};

#endif // TEXTINPUT_H
