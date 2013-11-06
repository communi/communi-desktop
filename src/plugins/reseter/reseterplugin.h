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

#ifndef RESETERPLUGIN_H
#define RESETERPLUGIN_H

#include <QtPlugin>
#include <QShortcut>
#include "treeplugin.h"

class ReseterPlugin : public QObject, public TreePlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")
#endif

public:
    ReseterPlugin(QObject* parent = 0);

    void initialize(TreeWidget* tree);

private slots:
    void resetItems();

private:
    struct Private {
        TreeWidget* tree;
        QShortcut* shortcut;
    } d;
};

#endif // RESETERPLUGIN_H
