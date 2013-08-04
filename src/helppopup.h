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

#ifndef HELPPOPUP_H
#define HELPPOPUP_H

#include <QLabel>

class HelpPopup : public QLabel
{
    Q_OBJECT

public:
    explicit HelpPopup(QWidget* parent = 0);
    ~HelpPopup();

public slots:
    void popup();

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
};

#endif // HELPPOPUP_H
