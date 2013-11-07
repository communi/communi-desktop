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

#ifndef TEXTDOCUMENTPLUGIN_H
#define TEXTDOCUMENTPLUGIN_H

#include <QtPlugin>

class TextDocument;

class TextDocumentPlugin
{
public:
    virtual ~TextDocumentPlugin() {}
    virtual void initialize(TextDocument*) {}
    virtual void uninitialize(TextDocument*) {}
};

Q_DECLARE_INTERFACE(TextDocumentPlugin, "Communi.TextDocumentPlugin")

#endif // TEXTDOCUMENTPLUGIN_H
