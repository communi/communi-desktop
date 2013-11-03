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

#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include <QLineEdit>

class TextBrowser;

class SearchInput : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchInput(TextBrowser* browser);

public slots:
    void find();
    void findNext();
    void findPrevious();

protected slots:
    void find(const QString& text, bool forward = false, bool backward = false, bool typed = true);

protected:
    void hideEvent(QHideEvent* event);

private:
    struct Private {
        TextBrowser* textBrowser;
    } d;
};

#endif // SEARCHINPUT_H
