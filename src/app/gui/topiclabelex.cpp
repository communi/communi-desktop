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

#include "topiclabelex.h"
#include "topicplugin.h"
#include <QPluginLoader> // TODO

// TODO:
Q_IMPORT_PLUGIN(SubjectPlugin)

TopicLabelEx::TopicLabelEx(QWidget* parent) : TopicLabel(parent)
{
    setOpenExternalLinks(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    // TODO: move outta here...
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        TopicPlugin* plugin = qobject_cast<TopicPlugin*>(instance);
        if (plugin)
            plugin->initialize(this);
    }
}
