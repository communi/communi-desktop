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

#ifndef WINDOWPLUGIN_H
#define WINDOWPLUGIN_H

#include <QtPlugin>
#include <QWidget>

class WindowPlugin
{
public:
    virtual ~WindowPlugin() {}
    virtual void initialize(QWidget*) {}
    virtual void uninitialize(QWidget*) {}
};

Q_DECLARE_INTERFACE(WindowPlugin, "Communi.WindowPlugin")

#endif // WINDOWPLUGIN_H
