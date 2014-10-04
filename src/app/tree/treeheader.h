/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef TREEHEADER_H
#define TREEHEADER_H

#include <QFrame>
#include <QStyle>

class TreeHeader : public QFrame
{
    Q_OBJECT

public:
    TreeHeader(QWidget* parent = 0);

    static TreeHeader* instance(QWidget* parent = 0);

    void setText(const QString& text) { d.text = text; }
    void setIcon(const QIcon& icon) { d.icon = icon; }
    void setState(QStyle::State state) { d.state = state; }

protected:
    void paintEvent(QPaintEvent* event);

private:
    struct Private {
        QIcon icon;
        QString text;
        QStyle::State state;
    } d;
};

#endif // TREEHEADER_H
