/****************************************************************************
 *  qtdockmanager.cpp
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

#include "qtdockmanager_p.h"
#include "qtdockprovider.h"
#include <QPluginLoader>
#include <QLibraryInfo>
#include <QDir>
#include <QLibrary>
#include <QApplication>
#include <QDebug>

#define _provider \
	if (!currentProvider()) \
		qWarning("Warning: cannot found usable dock provider. May be you forget make install command?"); \
	else \
		currentProvider()



QtDockManager *QtDockManager::instance()
{
	static QtDockManager pointer;
	return &pointer;
}

QtDockProvider *QtDockManager::currentProvider() const
{
	if (usableProviders().count())
		return usableProviders().first();
	return 0;
}

QtDockProviderList QtDockManager::usableProviders() const
{
	//TODO remove foreach
	QtDockProviderList list;
	foreach (QtDockProvider *provider, m_providers) {
		if (provider->isUsable())
			list.append(provider);
	}
	return list;
}

void QtDockManager::setMenu(QMenu *menu)
{
	m_menu = menu;
	_provider->setMenu(menu);
	emit menuChanged(menu);
}

QMenu *QtDockManager::menu() const
{
	return m_menu.data();
}

void QtDockManager::setBadge(const QString &text)
{
	m_badge = text;
	_provider->setBadge(text);
	emit badgeChanged(text);
}

QString QtDockManager::badge() const
{
	return m_badge;
}

void QtDockManager::setProgress(int percent)
{
	m_percent = percent;
	_provider->setProgress(percent);
	emit progressChanged(percent);
}

int QtDockManager::progress() const
{
	return m_percent;
}

void QtDockManager::alert(bool on)
{
    _provider->alert(on);
}

QVariant QtDockManager::platformInvoke(const QByteArray &method, const QVariant &arguments)
{
	QVariant result(QVariant::Invalid);
	QtDockProvider *provider = currentProvider();
	if (provider)
		result = provider->platformInvoke(method, arguments);
	return result;
}

QtDockManager::QtDockManager() : m_pluginLoader(new PluginLoader(QLatin1String("docktile"),
																 docktileProvider_iid))
{
	m_providers = m_pluginLoader->instances<QtDockProvider>();
}

QtDockManager::~QtDockManager()
{
}

void QtDockManager::addProvider(QtDockProvider *provider)
{
	m_providers.append(provider);
}

void QtDockManager::removeProvider(QtDockProvider *provider)
{
	m_providers.removeAll(provider);
}
