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

#ifndef OVERLAY_H
#define OVERLAY_H

#include <QLabel>
#include <QPointer>
#include <QToolButton>

class Overlay : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy)
    Q_PROPERTY(bool dark READ isDark WRITE setDark)
    Q_PROPERTY(bool refresh READ hasRefresh WRITE setRefresh)

public:
    explicit Overlay(QWidget* parent);

    bool isBusy() const;
    void setBusy(bool busy);

    bool isDark() const;
    void setDark(bool dark);

    bool hasRefresh() const;
    void setRefresh(bool enabled);

signals:
    void refresh();

protected:
    bool event(QEvent* event);
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void relayout();
    void reparent();

private:
    struct Private {
        bool dark;
        QToolButton* button;
        QPointer<QWidget> prevParent;
    } d;
};

#endif // OVERLAY_H
