/****************************************************************************
 *  temporaryicon.cpp
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

#include "temporaryicon.h"
#include <QFile>
#include <QImage>
#include <QApplication>
#include <QStyle>
#include <QUuid>
#include <QDesktopServices>

TemporaryIcon::TemporaryIcon(const QIcon &icon)
{
	int size = qApp->style()->pixelMetric(QStyle::PM_ListViewIconSize);
	m_iconSize = QSize(size, size);
	QImage image = icon.pixmap(m_iconSize).toImage();
	if (!image.isNull()) {
		QString name = QUuid::createUuid().toString();
		QString path = QDesktopServices::storageLocation(QDesktopServices::TempLocation);
		m_filePath = path + QLatin1String("/") + name + QLatin1String(".ico");
		image.save(m_filePath, "ico");
	}
}

TemporaryIcon::~TemporaryIcon()
{
	if (!m_filePath.isNull())
		QFile::remove(m_filePath);
}

QString TemporaryIcon::filePath() const
{
	return m_filePath;
}
