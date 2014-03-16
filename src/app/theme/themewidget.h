/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QLabel>
#include "themeinfo.h"

class ThemeWidget : public QLabel
{
    Q_OBJECT

public:
    ThemeWidget(const ThemeInfo& theme, QWidget* parent = 0);

private:
    struct Private {
        ThemeInfo theme;
    } d;
};

#endif // THEMEWIDGET_H
