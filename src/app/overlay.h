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

#ifndef OVERLAY_H
#define OVERLAY_H

#include <QFrame>
#include <QShortcut>

class IrcBuffer;
class BufferView;
class OverlayButton;

class Overlay : public QFrame
{
    Q_OBJECT

public:
    explicit Overlay(BufferView* parent);

    BufferView* view() const;

protected:
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void refresh();
    void relayout();
    void reconnect();
    void init(IrcBuffer* buffer);

private:
    struct Private {
        BufferView* view;
        IrcBuffer* buffer;
        QShortcut* shortcut;
        OverlayButton* button;
    } d;
};

#endif // OVERLAY_H
