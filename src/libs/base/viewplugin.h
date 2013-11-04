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

#ifndef VIEWPLUGIN_H
#define VIEWPLUGIN_H

#include <QtPlugin>

class SplitView;

class ViewPlugin
{
public:
    virtual ~ViewPlugin() {}
    virtual void initialize(SplitView*) {}
    virtual void uninitialize(SplitView*) {}
};

Q_DECLARE_INTERFACE(ViewPlugin, "com.github.communi.ViewPlugin")

#endif // VIEWPLUGIN_H
