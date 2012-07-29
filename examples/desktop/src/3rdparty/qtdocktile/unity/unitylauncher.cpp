/****************************************************************************
 *  unitylauncher.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <gorthauer87@ya.ru>
 *  Copyright (c) 2011 Vsevolod Velichko <torkvema@gmail.com>
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

#include "unitylauncher.h"
#include <QApplication>
#include <dbusmenuexporter.h>
#include <QDBusMessage>
#include <QtPlugin>

#include <QDebug>


template <typename T>
void UnityLauncher::sendMessage(const char *name, const T &value)
{
	QVariantMap map;
	map.insert(QLatin1String(name), value);
	sendMessage(map);
}

void UnityLauncher::sendMessage(const QVariantMap &map)
{
	QDBusMessage message = QDBusMessage::createSignal(appUri(), "com.canonical.Unity.LauncherEntry", "Update");
	QVariantList args;
	args << appDesktopUri()
			<< map;
	message.setArguments(args);
	if (!QDBusConnection::sessionBus().send(message))
		qWarning("Unable to send message");
}

UnityLauncher::UnityLauncher(QObject *parent) :
	QtDockProvider(parent)
{
	qDebug() << Q_FUNC_INFO;
}

UnityLauncher::~UnityLauncher()
{
	qDebug() << Q_FUNC_INFO;
}

bool UnityLauncher::isUsable() const
{
	return true; //TODO check com.canonical.Unity interface
}

void UnityLauncher::setMenu(QMenu *menu)
{
	if (m_menuExporter)
		m_menuExporter.data()->deleteLater();

	if (menu) {
		QString uri = appUri();
		m_menuExporter = new DBusMenuExporter(uri, menu);
		sendMessage("quicklist", uri);
	} else
		sendMessage("quicklist", QString());
}

void UnityLauncher::setBadge(const QString &badge)
{
	bool ok;
	qint64 count = badge.toInt(&ok);
	if (!ok)
		qWarning("Unity launcher doesn't support string badges");

	QVariantMap map;
	map.insert(QLatin1String("count"), count);
	map.insert(QLatin1String("count-visible"), count > 0);
	sendMessage(map);
}

void UnityLauncher::setProgress(int percents)
{
	int progress = qBound(0, percents, 100);
	QVariantMap map;
	map.insert(QLatin1String("progress"), static_cast<double>(progress)/100.0);
	map.insert(QLatin1String("progress-visible"), progress > 0);
	sendMessage(map);
}

void UnityLauncher::alert(bool on)
{
	sendMessage("urgent", on);
}

QString UnityLauncher::appDesktopUri() const
{
	return QString("application://%1.desktop").arg(qAppName());
}

QString UnityLauncher::appUri() const
{
	return QString("/%1").arg(qAppName());
}

Q_EXPORT_PLUGIN2(UnityLauncher, UnityLauncher)
