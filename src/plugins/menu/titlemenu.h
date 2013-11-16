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

#ifndef TITLEMENU_H
#define TITLEMENU_H

#include <QAction>

class TitleBar;
class SplitView;

class TitleMenu : public QObject
{
    Q_OBJECT

public:
    TitleMenu(TitleBar* bar, SplitView* view);

private slots:
    void updateActions();
    void splitVertical();
    void splitHorizontal();
    void unsplit();

private:
    struct Private {
        QAction* splitVAction;
        QAction* splitHAction;
        QAction* unsplitAction;
        SplitView* view;
        TitleBar* bar;
    } d;
};

#endif // BROWSERMENU_H
