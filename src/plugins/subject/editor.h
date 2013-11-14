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

#ifndef EDITOR_H
#define EDITOR_H

#include <QLabel>
#include <QTextEdit>

class TitleBar;

class Editor : public QTextEdit
{
    Q_OBJECT

public:
    Editor(TitleBar* parent = 0);

    bool eventFilter(QObject* object, QEvent* event);

public slots:
    void edit();

protected:
    bool event(QEvent* event);

private:
    struct Private {
        TitleBar* bar;
        QLabel* label;
    } d;
};

#endif // EDITOR_H
