/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "scrollbarstyle.h"
#include <QStyleOptionComplex>
#include <QStyleFactory>

ScrollBarStyle::ScrollBarStyle(bool expand) :
    QProxyStyle(QStyleFactory::create("fusion")), expand(expand)
{
}

ScrollBarStyle* ScrollBarStyle::narrow()
{
    static ScrollBarStyle style(false);
    return &style;
}

ScrollBarStyle* ScrollBarStyle::expanding()
{
    static ScrollBarStyle style(true);
    return &style;
}

int ScrollBarStyle::styleHint(StyleHint hint, const QStyleOption *option,
                              const QWidget *widget, QStyleHintReturn *returnData) const
{
    if (hint == SH_ScrollBar_Transient)
        return 1;
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void ScrollBarStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                                        QPainter* painter, const QWidget* widget) const
{
    if (!expand && control == CC_ScrollBar) {
        QStyleOptionComplex* complex = const_cast<QStyleOptionComplex*>(option);
        if (complex) {
            if (complex->activeSubControls != SC_None)
                complex->state |= State_On;
            complex->activeSubControls = SC_None;
        }
    }
    QProxyStyle::drawComplexControl(control, option, painter, widget);
}
