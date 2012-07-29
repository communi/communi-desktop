/****************************************************************************
 *  qtdocktile.cpp
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

#include "qtdocktile_p.h"
#include "qtdockmanager_p.h"
#include "qtdockprovider.h"
#include <QMenu>

static QtDockManager *manager()
{
    return QtDockManager::instance();
}

/*!
	\class QtDockTile
	\brief The QtDockTile class class provides an icon for an application in the dock


	\code
		QtDockTile *tile = new QtDockTile();
		tile->setMenu(menu);

		connect(lineEdit, SIGNAL(textChanged(QString)), tile, SLOT(setBadge(QString)));
		connect(horizontalSlider, SIGNAL(valueChanged(int)), tile, SLOT(setProgress(int)));
	\endcode

	\sa QtDockProvider
*/

/*!
	\reimp
 */

QtDockTile::QtDockTile(QObject *parent) :
    QObject(parent)
{
    connect(manager(), SIGNAL(badgeChanged(QString)), SIGNAL(badgeChanged(QString)));
    connect(manager(), SIGNAL(menuChanged(QMenu*)), SIGNAL(menuChanged(QMenu*)));
	connect(manager(), SIGNAL(progressChanged(int)), SIGNAL(progressChanged(int)));
}

QtDockTile::QtDockTile(QWidget *widget, QObject *parent) :
	QObject(parent ? parent : widget)
{
	manager()->platformInvoke("setWidget", qVariantFromValue(widget));

	connect(manager(), SIGNAL(badgeChanged(QString)), SIGNAL(badgeChanged(QString)));
	connect(manager(), SIGNAL(menuChanged(QMenu*)), SIGNAL(menuChanged(QMenu*)));
	connect(manager(), SIGNAL(progressChanged(int)), SIGNAL(progressChanged(int)));
}

/*!
	\reimp
 */

QtDockTile::~QtDockTile()
{
}

/*!
	TODO
*/

void QtDockTile::setMenu(QMenu *menu)
{
    manager()->setMenu(menu);
}

/*!
	\fn QtDockTile::menuChanged(QMenu *newMenu);

	QtDockTile emits this signal whenever the menu changes. \a newMenu is the new dock tile menu.

	\sa menu
*/

/*!
	\property QtDockTile::menu
	\brief The dock tile menu
	\return Current menu pointer
*/

QMenu *QtDockTile::menu() const
{
    return manager()->menu();
}

/*!
	TODO
	\a text
 */

void QtDockTile::setBadge(const QString &text)
{
    manager()->setBadge(text);
}

/*!
	\overload
	\a count - TODO
 */

void QtDockTile::setBadge(int count)
{
	manager()->setBadge(QString::number(count));
}

/*!
	\fn QtDockTile::badgeChanged(const QString &newBadge);

	QtDockTile emits this signal whenever the badge string changes. \a newBadge is the new dock tile badge string.

	\sa badge
*/

/*!
	\property QtDockTile::badge
	\brief The dock tile badge string
	\return Current badge string
*/

QString QtDockTile::badge() const
{
    return manager()->badge();
}

/*!
	TODO \a percent
*/

void QtDockTile::setProgress(int percent)
{
	manager()->setProgress(percent);
}

/*!
	\fn QtDockTile::progressChanged(int percent);

	QtDockTile emits this signal whenever the progress value changes. \a percent is the new dock progress value.

	\sa progress
*/

/*!
	\property QtDockTile::progress
	\brief The dock tile progress state.
	\return Current progress state in percents
*/

int QtDockTile::progress() const
{
	return manager()->progress();
}

/*!
	\brief Invoke platform depended methods in docktile provider
*/

QVariant QtDockTile::platformInvoke(const QByteArray &method, const QVariant &arguments)
{
	return manager()->platformInvoke(method, arguments);
}

/*!
	\brief TODO \a on
 */

void QtDockTile::alert(bool on)
{
	manager()->alert(on);
}
