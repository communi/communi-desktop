/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#ifndef HISTORYLINEEDIT_H
#define HISTORYLINEEDIT_H

#include <QStack>
#include <QLineEdit>

class HistoryLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	HistoryLineEdit(QWidget* parent = 0);

public slots:
    void goBackward();
    void goForward();
    void clearHistory();

protected:
	void keyPressEvent(QKeyEvent* event);

private:
    int index;
    QStringList history;
};

#endif // HISTORYLINEEDIT_H
