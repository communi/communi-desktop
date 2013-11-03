/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef BUFFERVIEWEX_H
#define BUFFERVIEWEX_H

#include "bufferview.h"

class BufferViewEx : public BufferView
{
    Q_OBJECT

public:
    explicit BufferViewEx(QWidget* parent = 0);

signals:
    void split(Qt::Orientation orientation);
};

#endif // BUFFERVIEWEX_H
