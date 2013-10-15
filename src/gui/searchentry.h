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

#ifndef SEARCHENTRY_H
#define SEARCHENTRY_H

#include "lineeditor.h"

QT_FORWARD_DECLARE_CLASS(QTextEdit)

class SearchEntry : public LineEditor
{
    Q_OBJECT

public:
    SearchEntry(QWidget* parent = 0);

    QTextEdit* textEdit() const;
    void setTextEdit(QTextEdit* textEdit);

public slots:
    void find();
    void findNext();
    void findPrevious();

protected slots:
    void find(const QString& text, bool forward = false, bool backward = false, bool typed = true);

protected:
    void hideEvent(QHideEvent* event);

private:
    struct Private {
        QTextEdit* textEdit;
    } d;
};

#endif // SEARCHENTRY_H
