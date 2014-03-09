/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef LISTFINDER_H
#define LISTFINDER_H

#include "finder.h"

class ListView;

class ListFinder : public Finder
{
    Q_OBJECT

public:
    explicit ListFinder(ListView* list);

protected slots:
    void find(const QString& text, bool forward = false, bool backward = false, bool typed = true);
    void relocate();

private slots:
    void onReturnPressed();

private:
    struct Private {
        ListView* list;
    } d;
};

#endif // LISTFINDER_H
