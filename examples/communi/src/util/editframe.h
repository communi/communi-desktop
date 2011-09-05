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

#ifndef EDITFRAME_H
#define EDITFRAME_H

#include "ui_editframe.h"
class HistoryLineEdit;
class Completer;

class EditFrame : public QFrame
{
    Q_OBJECT

public:
    EditFrame(QWidget* parent = 0);

    Completer* completer() const;
    HistoryLineEdit* lineEdit() const;

signals:
    void send(const QString& text);
    void typed(const QString& text);

private slots:
    void onSend();
    void updateUi();

private:
    struct EditFrameUi : public Ui::EditFrame
    {
        Completer* completer;
    } ui;
};

#endif // EDITFRAME_H
