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

#include "themeplugin.h"
#include "treedelegate.h"
#include "textdocument.h"
#include "treewidget.h"
#include "mainwindow.h"
#include <QApplication>

inline void initResources()
{
    Q_INIT_RESOURCE(dark);
    Q_INIT_RESOURCE(default);
}

ThemePlugin::ThemePlugin(QObject* parent) : QObject(parent)
{
    initResources();
    d.theme.load(":/themes/dark/dark.theme");
    //d.theme.load(":/themes/default/default.theme");
}

void ThemePlugin::initialize(TextDocument* doc)
{
    doc->setDefaultStyleSheet(d.theme.docStyleSheet());
}

void ThemePlugin::initialize(TreeWidget* tree)
{
    // TODO
    TreeDelegate* delegate = new TreeDelegate(tree);
    tree->setItemDelegate(delegate);
}

void ThemePlugin::initialize(MainWindow* window)
{
    // TODO: app or window palette? would it make sense to keep the app palette pristine,
    //       so that it can be restored, and only customize the window palette instead?
    //       ...or even apply to all widgets?
    qApp->setPalette(d.theme.appPalette());
    window->setStyleSheet(d.theme.appStyleSheet());
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(ThemePlugin)
#endif
