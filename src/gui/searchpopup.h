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

#ifndef SEARCHPOPUP_H
#define SEARCHPOPUP_H

#include <QLineEdit>
#include <QTreeWidget>

class SearchPopup : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchPopup(QTreeWidget* parent = 0);
    ~SearchPopup();

public slots:
    void popup();

protected:
    void mousePressEvent(QMouseEvent* event);

private slots:
    void search(const QString& text);
    void searchAgain();

private:
    struct Private {
        QTreeWidget* tree;
    } d;
};

#endif // SEARCHPOPUP_H
