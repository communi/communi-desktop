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

#ifndef FINDER_H
#define FINDER_H

#include <QLineEdit>
#include <QTreeWidget>
#include "treeextension.h"

class Finder : public QLineEdit, public TreeExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(TreeExtensionInterface)

public:
    explicit Finder(QObject* parent = 0);

    void initialize(QTreeWidget* tree);

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

#endif // FINDER_H
