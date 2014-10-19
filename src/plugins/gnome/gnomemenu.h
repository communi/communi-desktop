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

#ifndef GNOMEMENU_H
#define GNOMEMENU_H

#ifdef COMMUNI_HAVE_GIO

#include <QObject>
#include <QWidget>
#include <QScopedPointer>

class GnomeMenuPrivate;

class GnomeMenu : public QObject
{
    Q_OBJECT

public:
    explicit GnomeMenu(QObject* parent = 0);
    ~GnomeMenu();

    void addSimpleItem(const QByteArray& id, const QString& caption, QObject* receiver, const char* slot);
    void addToggleItem(const QByteArray& id, const QString& caption, bool isChecked, QObject* receiver, const char* slot);
    void setToggleItemState(const QByteArray& id, bool checked);
    void addSection(GnomeMenu* section);

    void setMenuToWindow(WId winId, const QByteArray& dbusPathPrefix);

private:
    QScopedPointer<GnomeMenuPrivate> d;
};

#endif // COMMUNI_HAVE_GIO

#endif // GNOMEMENU_H
