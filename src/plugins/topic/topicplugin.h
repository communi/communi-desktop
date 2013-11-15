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

#ifndef TOPICPLUGIN_H
#define TOPICPLUGIN_H

#include <QtPlugin>
#include "bufferviewplugin.h"

class TopicPlugin : public QObject, public BufferViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(BufferViewPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.BufferViewPlugin")
#endif

public:
    TopicPlugin(QObject* parent = 0);

    void initialize(BufferView* view);
};

#endif // TOPICPLUGIN_H
