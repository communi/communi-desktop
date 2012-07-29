/****************************************************************************
 *  qtdockprovider.cpp
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

#include "qtdockprovider.h"
#include "qtdockmanager_p.h"
#include <QApplication>

/*!
	\class QtDockProviderInterface
	\brief TODO

*/

/*!
	\fn QtDockProviderInterface::isUsable
	TODO
	\return
 */

QtDockProvider::QtDockProvider(QObject *parent) :
	QObject(parent)
{
}

QMenu *QtDockProvider::menu() const
{
    return QtDockManager::instance()->menu();
}

QString QtDockProvider::badge() const
{
    return QtDockManager::instance()->badge();
}

int QtDockProvider::progress() const
{
	return QtDockManager::instance()->progress();
}

QWidget *QtDockProvider::window() const
{
	return qApp->activeWindow();
}

QVariant QtDockProviderInterface::platformInvoke(const QByteArray &, const QVariant &)
{
	return QVariant::Invalid;
}
