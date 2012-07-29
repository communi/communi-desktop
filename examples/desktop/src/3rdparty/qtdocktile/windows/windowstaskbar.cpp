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

#include "windowstaskbar.h"
#include <QSysInfo>
#include <QtPlugin>
#include "wrapper/taskbar.h"
#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QDebug>
#include "jumplistsmenuexporter.h"

WindowsTaskBar::WindowsTaskBar(QObject *parent) :
	QtDockProvider(parent),
	m_menuExporter(new JumpListsMenuExporter(this))
{
	qDebug() << Q_FUNC_INFO;
	connect(qApp, SIGNAL(aboutToQuit()), SLOT(onAboutToQuit()));
}

WindowsTaskBar::~WindowsTaskBar()
{
}

bool WindowsTaskBar::isUsable() const
{
	return QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7;
}

void WindowsTaskBar::setMenu(QMenu *menu)
{
	m_menuExporter->setMenu(menu);
}

void WindowsTaskBar::setBadge(const QString &badge)
{
	if (badge.isEmpty())
		clearOverlayIcon(window()->winId());
	else {
		QPixmap pixmap = createBadge(badge);
		setOverlayIcon(window()->winId(), pixmap.toWinHICON());
	}
}

void WindowsTaskBar::setProgress(int percents)
{
	int progress = qBound(0, percents, 100);
	setProgressValue(window()->winId(), progress);
}

void WindowsTaskBar::alert(bool on)
{
	if (on) {
		FLASHWINFO fi = {0};
		fi.cbSize  = sizeof(fi);
		fi.dwFlags = FLASHW_TRAY;
		fi.hwnd    = window()->winId();
		fi.uCount  = 1;
		FlashWindowEx(&fi);
	}
}

QVariant WindowsTaskBar::platformInvoke(const QByteArray &method, const QVariant &arguments)
{
	if (method == "setWidget") {
		m_widget = arguments.value<QWidget*>();
		return true;
	}
	return QtDockProvider::platformInvoke(method, arguments);
}

QWidget *WindowsTaskBar::window() const
{
	return m_widget.isNull() ? QtDockProvider::window() : m_widget.data();
}

QPixmap WindowsTaskBar::createBadge(const QString &badge) const
{
	QPixmap pixmap(overlayIconSize());
	QRect rect = pixmap.rect();
	rect.adjust(1, 1, -1, -1);
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	QPalette palette = window()->palette();
	painter.setBrush(palette.toolTipBase());

	QPen pen = painter.pen();
	pen.setColor(palette.color(QPalette::ToolTipText));
	painter.setPen(pen);

	QString label = QFontMetrics(painter.font()).elidedText(badge, Qt::ElideMiddle, rect.width());
	painter.drawRoundedRect(rect, 5, 5);
	painter.drawText(rect,
					 Qt::AlignCenter | Qt::TextSingleLine,
					 label);
	return pixmap;
}

QSize WindowsTaskBar::overlayIconSize() const
{
	int size = qApp->style()->pixelMetric(QStyle::PM_ListViewIconSize);
	return QSize(size, size);
}

void WindowsTaskBar::onAboutToQuit()
{
	deleteJumpLists();
}

Q_EXPORT_PLUGIN2(WindowsTaskBar, WindowsTaskBar)
