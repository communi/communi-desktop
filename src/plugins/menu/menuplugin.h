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

#ifndef MENUPLUGIN_H
#define MENUPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "bufferviewplugin.h"
#include "splitviewplugin.h"

class MenuPlugin : public QObject, public BufferViewPlugin,
                                   public SplitViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(BufferViewPlugin SplitViewPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.BufferViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.SplitViewPlugin")
#endif

public:
    MenuPlugin(QObject* parent = 0);

    void initView(BufferView* view);
    void initView(SplitView* view);

private:
    struct Private {
        SplitView* view;
    } d;
};

#endif // MENUPLUGIN_H
