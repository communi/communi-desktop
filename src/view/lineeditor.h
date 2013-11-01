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

#ifndef LINEEDITOR_H
#define LINEEDITOR_H

#include "fancylineedit.h"

class LineEditor : public Utils::FancyLineEdit
{
    Q_OBJECT

public:
    explicit LineEditor(QWidget* parent = 0);

    QStringList history() const;

public slots:
    void goBackward();
    void goForward();

    void setHistory(const QStringList& history);
    void clearHistory();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    struct Private {
        int index;
        QString input;
        QStringList history;
    } d;
};

#endif // LINEEDITOR_H
