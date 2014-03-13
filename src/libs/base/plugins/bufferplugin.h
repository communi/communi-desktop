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

#ifndef BUFFERPLUGIN_H
#define BUFFERPLUGIN_H

#include <QtPlugin>

class IrcBuffer;

class BufferPlugin
{
public:
    virtual ~BufferPlugin() {}
    virtual void initBuffer(IrcBuffer*) {}
    virtual void cleanupBuffer(IrcBuffer*) {}
};

Q_DECLARE_INTERFACE(BufferPlugin, "Communi.BufferPlugin")

#endif // BUFFERPLUGIN_H
