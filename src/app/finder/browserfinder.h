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

#ifndef BROWSERFINDER_H
#define BROWSERFINDER_H

#include "abstractfinder.h"

class TextBrowser;

class BrowserFinder : public AbstractFinder
{
    Q_OBJECT

public:
    explicit BrowserFinder(TextBrowser* browser);
    ~BrowserFinder();

    void setVisible(bool visible);

protected slots:
    void find(const QString& text, bool forward = false, bool backward = false, bool typed = true);
    void relocate();

private:
    struct Private {
        TextBrowser* textBrowser;
    } d;
};

#endif // BROWSERFINDER_H
