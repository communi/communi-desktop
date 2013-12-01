/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef THEMELOADER_H
#define THEMELOADER_H

#include <QObject>

class ThemeLoader : public QObject
{
    Q_OBJECT

public:
    static ThemeLoader* instance();

private:
    ThemeLoader(QObject* parent = 0);
};

#endif // THEMELOADER_H
