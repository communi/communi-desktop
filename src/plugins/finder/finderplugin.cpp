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

#include "finderplugin.h"
#include "searchpopup.h"
#include "searchinput.h"

FinderPlugin::FinderPlugin(QObject* parent) : QObject(parent)
{
}

void FinderPlugin::initialize(TreeWidget* tree)
{
    new SearchPopup(tree);
}

void FinderPlugin::initialize(TextBrowser* browser)
{
    new SearchInput(browser);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(FinderPlugin)
#endif
