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

#ifndef TREEBADGE_H
#define TREEBADGE_H

#include <QLabel>

class TreeBadge : public QLabel
{
    Q_OBJECT

public:
    TreeBadge(QWidget* parent = 0);

    static TreeBadge* instance(QWidget* parent = 0);

    void setNum(int num);
    void setHighlighted(int hilite) { d.hilite = hilite; }

protected:
    void paintEvent(QPaintEvent* event);
    void drawBackground(QPainter* painter);

private:
    struct Private {
        int num;
        bool hilite;
    } d;
};

#endif // TREEBADGE_H
