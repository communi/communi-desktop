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

#ifndef TREEEXTENSION_H
#define TREEEXTENSION_H

#include <QtPlugin>
#include <QTreeWidget>

class TreeExtensionInterface
{
public:
    virtual ~TreeExtensionInterface() {}
    virtual void initialize(QTreeWidget* tree) = 0;
};

Q_DECLARE_INTERFACE(TreeExtensionInterface, "com.github.communi.TreeExtensionInterface")

#endif // TREEEXTENSION_H
