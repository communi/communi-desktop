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
#include "textbrowser.h"
#include "textdocument.h"
#include "textinput.h"
#include "listview.h"
#include "titlebar.h"
#include "treewidget.h"
#include "bufferview.h"
#include "mainwindow.h"

inline void initResource() { Q_INIT_RESOURCE(default); }

ThemePlugin::ThemePlugin(QObject* parent) : QObject(parent)
{
    initResource();
}

void ThemePlugin::initialize(BufferView* view)
{
    view->textBrowser()->setStyleSheet(
        "TextBrowser {"
            "border: none;"
            "color: #000000;"
            "background: #ffffff;"
            "selection-color: palette(highlighted-text);"
            "selection-background-color: palette(highlight);"
        "}");

    view->textInput()->setStyleSheet(
        "TextInput {"
            "border: 1px solid transparent;"
            "border-top-color: palette(dark);"
        "}");

    view->listView()->setStyleSheet(
        "ListView {"
            "border: none;"
            "background: palette(alternate-base);"
            "selection-background-color: palette(highlight);"
        "}"
        "ListView::item {"
            "height: 20px;"
        "}");

    QLabel* label = view->titleBar()->findChild<QLabel*>("title");
    QTextDocument* doc = label->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet("a { color: #4040ff }");

    view->titleBar()->setStyleSheet(
        "TitleBar {"
            "min-height: 18px;"
            "border: 1px solid transparent;"
            "border-bottom-color: palette(dark);"
            "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                        "stop: 0.0 palette(light),"
                                        "stop: 1.0 palette(button));"
        "}"
        "TitleBar > QTextEdit {"
            "border: 1px solid transparent;"
            "border-bottom-color: palette(dark);"
            "selection-color: palette(highlighted-text);"
            "selection-background-color: palette(highlight);"
        "}");
}

void ThemePlugin::initialize(TextDocument* doc)
{
    doc->setDefaultStyleSheet(
        ".highlight { color: #ff4040 }"
        ".notice    { color: #a54242 }"
        ".action    { color: #8b388b }"
        ".event     { color: #808080 }"
        ".timestamp { color: #808080; font-size: small }"
        "a { color: #4040ff }");
}

void ThemePlugin::initialize(TreeWidget* tree)
{
    TreeDelegate* delegate = new TreeDelegate(tree);
    tree->setItemDelegate(delegate);

    tree->setStyleSheet(
        "TreeWidget {"
            "border: none;"
            "background: palette(alternate-base);"
            "selection-background-color: palette(highlight);"
        "}"
        "TreeWidget::item {"
            "height: 20px;"
        "}");
}

void ThemePlugin::initialize(MainWindow* window)
{
    window->setStyleSheet(
        "QSplitter::handle {"
            "width: 1px;"
            "height: 1px;"
            "background: palette(dark);"
        "}"
        "QToolButton {"
            "border: 1px solid palette(dark);"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                              "stop: 0 palette(light), stop: 1 palette(button));"
        "}"
        "QToolButton:pressed {"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                              "stop: 0 palette(button), stop: 1 palette(light));"
        "}"
        "QToolButton::menu-indicator {"
            "image: none;"
        "}"
        "TitleBar > QToolButton {"
            "min-width: 20px;"
            "max-width: 20px;"
            "min-height: 18px;"
            "max-height: 18px;"
            "border-top-color: transparent;"
            "border-right-color: transparent;"
        "}"
        "TitleBar > QToolButton#menu {"
            "image: url(:/images/down.png)"
        "}"
        "TitleBar > QToolButton#close {"
            "image: url(:/images/close.png)"
        "}"
        "Finder > QToolButton {"
            "min-width: 20px;"
            "max-width: 20px;"
            "min-height: 20px;"
            "max-height: 20px;"
        "}"
        "Finder > QLineEdit {"
            "border: 1px solid palette(dark);"
            "border-bottom-color: transparent;"
        "}"
        "Finder > QToolButton {"
            "border-left-color: transparent;"
        "}"
        "Finder > QToolButton#prev {"
            "image: url(:/images/up.png)"
        "}"
        "Finder > QToolButton#next {"
            "image: url(:/images/down.png)"
        "}");
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(ThemePlugin)
#endif
