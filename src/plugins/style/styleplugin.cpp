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

#include "styleplugin.h"
#include "treedelegate.h"
#include "itemdelegate.h"
#include "textbrowser.h"
#include "textdocument.h"
#include "textinput.h"
#include "listview.h"
#include "titlebar.h"
#include "treewidget.h"
#include "bufferview.h"
#include "window.h"

static int baseHeight()
{
    // TODO:
    QLineEdit lineEdit;
    lineEdit.setStyleSheet("QLineEdit { border: 1px solid transparent; }");
    return lineEdit.sizeHint().height();
}

StylePlugin::StylePlugin(QObject* parent) : QObject(parent)
{
}

void StylePlugin::initialize(BufferView* view)
{
    view->textBrowser()->setStyleSheet(
        "TextBrowser {"
        "    border: none;"
        "    color: #000000;"
        "    background: #ffffff;"
        "    selection-color: palette(highlighted-text);"
        "    selection-background-color: palette(highlight);"
        "}");

    view->textInput()->setStyleSheet(
        "TextInput {"
        "    border: 1px solid transparent;"
        "    border-top-color: palette(dark);"
        "}");

    ItemDelegate* delegate = new ItemDelegate(view->listView());
    delegate->setItemHeight(baseHeight());
    view->listView()->setItemDelegate(delegate);

    view->listView()->setStyleSheet(
        "ListView {"
        "    border: none;"
        "    background: palette(alternate-base);"
        "    selection-background-color: palette(highlight);"
        "}");

    QLabel* label = view->titleBar()->findChild<QLabel*>("title");
    QTextDocument* doc = label->findChild<QTextDocument*>();
    if (doc)
        doc->setDefaultStyleSheet("a { color: #4040ff }");

    view->titleBar()->setMinimumHeight(baseHeight());

    view->titleBar()->setStyleSheet(
        "TitleBar {"
        "    border: 1px solid transparent;"
        "    border-bottom-color: palette(dark);"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                stop: 0.0 palette(light),"
        "                                stop: 1.0 palette(button));"
        "}"
        "TitleBar > QTextEdit {"
        "    border: 1px solid transparent;"
        "    border-bottom-color: palette(dark);"
        "    selection-color: palette(highlighted-text);"
        "    selection-background-color: palette(highlight);"
        "}");
}

void StylePlugin::initialize(TextDocument* doc)
{
    doc->setDefaultStyleSheet(
        ".highlight { color: #ff4040 }"
        ".notice    { color: #a54242 }"
        ".action    { color: #8b388b }"
        ".event     { color: #808080 }"
        ".timestamp { color: #808080; font-size: small }"
        "a { color: #4040ff }");
}

void StylePlugin::initialize(TreeWidget* tree)
{
    TreeDelegate* delegate = new TreeDelegate(tree);
    delegate->setItemHeight(baseHeight());
    tree->setItemDelegate(delegate);

    tree->setStyleSheet(
        "TreeWidget {"
        "    border: none;"
        "    background: palette(alternate-base);"
        "    selection-background-color: palette(highlight);"
        "}");
}

void StylePlugin::initialize(Window* window)
{
    window->setStyleSheet(
        "QSplitter::handle {"
        "    width: 1px;"
        "    height: 1px;"
        "    background: palette(dark);"
        "}"
        "Finder > QLineEdit {"
            "border: 1px solid palette(dark);"
            "border-bottom-color: transparent;"
        "}"
        "Finder > QToolButton {"
            "border: 1px solid palette(dark);"
            "border-left-color: transparent;"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                              "stop: 0 palette(light), stop: 1 palette(button));"
        "}"
        "Finder > QToolButton:pressed {"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                              "stop: 0 palette(button), stop: 1 palette(light));"
        "}");
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(StylePlugin)
#endif
