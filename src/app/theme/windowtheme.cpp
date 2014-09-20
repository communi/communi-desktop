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

#include "windowtheme.h"
#include "themeinfo.h"

#ifdef QT_X11EXTRAS_LIB
#include <X11/Xlib.h>
#include <QX11Info>
#include <QLibrary>

static bool setGtkTheme(WId winId, const QByteArray& theme)
{
    typedef Atom (*XInternAtomFunc)(Display*, const char*, int);
    typedef void (*XChangePropertyFunc)(Display*, Window, Atom, Atom, int, int, const unsigned char*, int);
    typedef void (*XDeletePropertyFunc)(Display*, Window, Atom);

    QLibrary xlib("libX11.so");
    if (!xlib.load())
        return false;

    XInternAtomFunc xInternAtom = (XInternAtomFunc) xlib.resolve("XInternAtom");
    XChangePropertyFunc xChangeProperty = (XChangePropertyFunc) xlib.resolve("XChangeProperty");
    XDeletePropertyFunc xDeleteProperty = (XDeletePropertyFunc) xlib.resolve("XDeleteProperty");
    if (!xInternAtom || !xChangeProperty || !xDeleteProperty)
        return false;

    if (!theme.isEmpty()) {
        xChangeProperty(QX11Info::display(), winId,
                        xInternAtom(QX11Info::display(), "_GTK_THEME_VARIANT", false),
                        xInternAtom(QX11Info::display(), "UTF8_STRING", false),
                        8, PropModeReplace, (uchar*) theme.data(), theme.length());
    } else {
        xDeleteProperty(QX11Info::display(), winId,
                        xInternAtom(QX11Info::display(), "_GTK_THEME_VARIANT", false));
    }
    return true;
}
#endif

void WindowTheme::setTheme(QWidget* window, const ThemeInfo& theme)
{
    window->setStyleSheet(theme.style());

#ifdef QT_X11EXTRAS_LIB
    setGtkTheme(window->winId(), theme.gtkTheme().toUtf8());
#endif
}
