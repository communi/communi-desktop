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

#include "x11helper.h"
#include <X11/Xlib.h>
#include <QX11Info>

namespace X11Helper
{
    bool setWindowProperty(WId winId, const QByteArray& name, const QByteArray& value)
    {
        if (!value.isEmpty()) {
            XChangeProperty(QX11Info::display(), winId,
                            XInternAtom(QX11Info::display(), name.data(), false),
                            XInternAtom(QX11Info::display(), "UTF8_STRING", false),
                            8, PropModeReplace, (uchar*) value.data(), value.length());
        } else {
            XDeleteProperty(QX11Info::display(), winId,
                            XInternAtom(QX11Info::display(), name.data(), false));
        }
        return true;
    }
}
