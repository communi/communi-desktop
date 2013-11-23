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

#include "highlighterplugin.h"
#include "documenthighlighter.h"
#include "treehighlighter.h"

HighlighterPlugin::HighlighterPlugin(QObject* parent) : QObject(parent)
{
}

void HighlighterPlugin::initDocument(TextDocument* document)
{
    new DocumentHighlighter(document);
}

void HighlighterPlugin::initTree(TreeWidget* tree)
{
    new TreeHighlighter(tree);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(HighlighterPlugin)
#endif
