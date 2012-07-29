/****************************************************************************
 *  unitylauncher.h
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

#ifndef UNITYLAUNCHER_H
#define UNITYLAUNCHER_H
#include <qtdockprovider.h>
#include <QWeakPointer>
#include <QVariant>

class DBusMenuExporter;
class UnityLauncher : public QtDockProvider
{
	Q_OBJECT
public:
	explicit UnityLauncher(QObject *parent = 0);
	virtual ~UnityLauncher();
	virtual bool isUsable() const;
	virtual void setMenu(QMenu *menu);
	virtual void setBadge(const QString &badge);
	virtual void setProgress(int percents);
	virtual void alert(bool on);
protected:
	QString appDesktopUri() const;
	QString appUri() const;
private:
	template <typename T>
	void sendMessage(const char *method, const T &value);
	void sendMessage(const QVariantMap &properties);
	QWeakPointer<DBusMenuExporter> m_menuExporter;
};

#endif // UNITYLAUNCHER_H
