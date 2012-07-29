/****************************************************************************
 *  windowstaskbar.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <gorthauer87@ya.ru>
 *
 ***************************************************************************
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
*****************************************************************************/

#define WINVER 0x0500
#include <qt_windows.h>

#include "qtdocktile.h"
#include "qtdocktile_p.h"
#include "taskbar.h"
#include <QApplication>
#include <QSysInfo>
#include <QPainter>
#include <QStyle>

static QPixmap createBadge(const QString &badge, const QPalette &palette)
{
    int size = qApp->style()->pixelMetric(QStyle::PM_ListViewIconSize);
    QPixmap pixmap(size, size);
    QRect rect = pixmap.rect();
    rect.adjust(1, 1, -1, -1);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(palette.toolTipBase());

    QPen pen = painter.pen();
    pen.setColor(palette.color(QPalette::ToolTipText));
    painter.setPen(pen);

    QString label = QFontMetrics(painter.font()).elidedText(badge, Qt::ElideMiddle, rect.width());
    painter.drawRoundedRect(rect, 5, 5);
    painter.drawText(rect, Qt::AlignCenter | Qt::TextSingleLine, label);
    return pixmap;
}

bool QtDockTile::isAvailable()
{
    return QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7;
}

void QtDockTilePrivate::setBadge(const QString &badge)
{
    if (badge.isEmpty())
        clearOverlayIcon(window->winId());
    else {
        QPixmap pixmap = createBadge(badge, window->palette());
        setOverlayIcon(window->winId(), pixmap.toWinHICON());
    }
}

void QtDockTilePrivate::setProgress(int progress)
{
    setProgressValue(window->winId(), progress);
}

void QtDockTilePrivate::alert(bool on)
{
    if (on) {
        FLASHWINFO fi = {0};
        fi.cbSize  = sizeof(fi);
        fi.dwFlags = FLASHW_TRAY;
        fi.hwnd    = window->winId();
        fi.uCount  = 1;
        FlashWindowEx(&fi);
    }
}
