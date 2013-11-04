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

class TopicLabel;

class TopicPlugin
{
public:
    virtual ~TopicPlugin() {}
    virtual void initialize(TopicLabel*) {}
    virtual void uninitialize(TopicLabel*) {}
};

Q_DECLARE_INTERFACE(TopicPlugin, "com.github.communi.TopicPlugin")

#endif // TOPICPLUGIN_H
