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

#include "titleplugin.h"
#include "titleextension.h"
#include "bufferview.h"

TitlePlugin::TitlePlugin(QObject* parent) : QObject(parent)
{
}

void TitlePlugin::initView(BufferView* view)
{
    new TitleExtension(view->titleBar());
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(TitlePlugin)
#endif
