/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef SCROLLBARSTYLE_H
#define SCROLLBARSTYLE_H

#include <QProxyStyle>

class ScrollBarStyle : public QProxyStyle
{
public:
    static ScrollBarStyle* narrow();
    static ScrollBarStyle* expanding();

    int styleHint(StyleHint hint, const QStyleOption *option = 0,
                  const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                            QPainter* painter, const QWidget* widget = 0) const;

private:
    ScrollBarStyle(bool expand);
    bool expand;
};

#endif // SCROLLBARSTYLE_H
