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

#include "titlemenu.h"
#include "bufferview.h"
#include "splitview.h"
#include "titlebar.h"
#include <QContextMenuEvent>

TitleMenu::TitleMenu(TitleBar* bar, SplitView* view) : QObject(bar)
{
    d.view = view;
    d.bar = bar;

    connect(d.view, SIGNAL(viewAdded(BufferView*)), this, SLOT(updateActions()));
    connect(d.view, SIGNAL(viewRemoved(BufferView*)), this, SLOT(updateActions()));

    d.splitVAction = new QAction(tr("Split"), bar);
    connect(d.splitVAction, SIGNAL(triggered()), this, SLOT(splitVertical()));
    bar->addAction(d.splitVAction);

    d.splitHAction = new QAction(tr("Split side by side"), bar);
    connect(d.splitHAction, SIGNAL(triggered()), this, SLOT(splitHorizontal()));
    bar->addAction(d.splitHAction);

    d.unsplitAction = new QAction(tr("Unsplit"), bar);
    connect(d.unsplitAction, SIGNAL(triggered()), this, SLOT(unsplit()));
    bar->addAction(d.unsplitAction);

    updateActions();
}

void TitleMenu::updateActions()
{
    d.unsplitAction->setEnabled(d.view->views().count() > 1);
}

void TitleMenu::splitVertical()
{
    d.view->split(Qt::Vertical);
}

void TitleMenu::splitHorizontal()
{
    d.view->split(Qt::Horizontal);
}

void TitleMenu::unsplit()
{
    QWidget* parent = d.bar->parentWidget();
    while (parent) {
        if (qobject_cast<BufferView*>(parent)) {
            parent->deleteLater();
            break;
        }
        parent = parent->parentWidget();
    }
}
