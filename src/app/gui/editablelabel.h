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

#ifndef EDITABLELABEL_H
#define EDITABLELABEL_H

#include <QLabel>
#include <QTextEdit>

class EditableLabel : public QLabel
{
    Q_OBJECT

public:
    EditableLabel(QWidget* parent = 0);

public slots:
    void edit();

signals:
    void edited(const QString& text);

protected:
    bool eventFilter(QObject* object, QEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    struct Private {
        QTextEdit* editor;
    } d;
};

#endif // EDITABLELABEL_H
