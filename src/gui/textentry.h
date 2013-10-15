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

#ifndef TEXTENTRY_H
#define TEXTENTRY_H

#include "lineeditor.h"

class Completer;
class IrcBuffer;
class IrcUserModel;
class CommandParser;

class TextEntry : public LineEditor
{
    Q_OBJECT

public:
    TextEntry(QWidget* parent = 0);

    IrcBuffer* currentBuffer() const;

public slots:
    void setCurrentBuffer(IrcBuffer* buffer);

private slots:
    void sendInput();
    void cleanup(IrcBuffer* buffer);

private:
    struct Private {
        IrcBuffer* buffer;
        Completer* completer;
        CommandParser* parser;
        IrcUserModel* userModel;
        QHash<IrcBuffer*, QStringList> histories;
    } d;
};

#endif // TEXTENTRY_H
