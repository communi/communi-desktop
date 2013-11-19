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

#ifndef TITLEEXTENSION_H
#define TITLEEXTENSION_H

#include <QToolButton>

class TitleBar;

class TitleExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    TitleExtension(TitleBar* bar);

    int offset() const;
    void setOffset(int offset);

    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void aboutToShowMenu();
    void aboutToHideMenu();
    void updateButtons();

    void animateShow();
    void animateHide();

    void relocate();

private:
    struct Private {
        bool menu;
        int offset;
        TitleBar* bar;
        QToolButton* menuButton;
        QToolButton* closeButton;
    } d;
};

#endif // TITLEEXTENSION_H
