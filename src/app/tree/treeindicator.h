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

#ifndef TREEINDICATOR_H
#define TREEINDICATOR_H

#include <QFrame>
#include <QStyle>

class TreeIndicator : public QFrame
{
    Q_OBJECT

public:
    TreeIndicator(QWidget* parent = 0);

    static TreeIndicator* instance(QWidget* parent = 0);

    void setLag(qint64 lag) { d.lag = lag; }
    void setState(QStyle::State state) { d.state = state; }

protected:
    void paintEvent(QPaintEvent* event);
    void drawBackground(QPainter* painter);

private:
    struct Private {
        qint64 lag;
        QStyle::State state;
    } d;
};

#endif // TREEINDICATOR_H
