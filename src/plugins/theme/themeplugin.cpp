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
#include "textdocument.h"
#include "mainwindow.h"
#include "bufferview.h"
#include "titlebar.h"
#include <QApplication>

inline void initResources()
{
    Q_INIT_RESOURCE(cute);
    Q_INIT_RESOURCE(dark);
}

ThemePlugin::ThemePlugin(QObject* parent) : QObject(parent)
{
    initResources();
    d.theme.load(":/themes/cute/cute.theme");
    //d.theme.load(":/themes/dark/dark.theme");
}

void ThemePlugin::initWindow(MainWindow* window)
{
    window->setStyleSheet(d.theme.appStyleSheet());
}

void ThemePlugin::initDocument(TextDocument* doc)
{
    doc->setStyleSheet(d.theme.docStyleSheet());
}

void ThemePlugin::initView(BufferView* view)
{
    TitleBar* bar = view->titleBar();
    QTextDocument* doc = bar->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet(d.theme.docStyleSheet());
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(ThemePlugin)
#endif
