/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>
class QLabel;

class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    WelcomePage(QWidget* parent = 0);

signals:
    void connectRequested();

private slots:
    void updateHtml();

private:
    QWidget* createBody(QWidget* parent = 0) const;

    QLabel* header;
    QLabel* footer;
};

#endif // WELCOMEPAGE_H
