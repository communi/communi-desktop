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

class SearchPopup : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchPopup(QWidget* parent = 0);
    ~SearchPopup();

public slots:
    void popup();

signals:
    void searched(const QString& search);
    void searchedAgain(const QString& search);

protected:
    void mousePressEvent(QMouseEvent* event);

private slots:
    void onReturnPressed();
    void onSearched(bool result);
};

#endif // SEARCHPOPUP_H
