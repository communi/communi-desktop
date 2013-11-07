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

#include "subjectplugin.h"
#include "bufferview.h"
#include "editor.h"

SubjectPlugin::SubjectPlugin(QObject* parent) : QObject(parent)
{
}

void SubjectPlugin::initialize(BufferView* view)
{
    new Editor(view->topicLabel());
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(SubjectPlugin)
#endif
