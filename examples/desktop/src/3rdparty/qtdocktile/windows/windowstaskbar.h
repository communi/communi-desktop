/****************************************************************************
 *  windowstaskbar.h
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

#ifndef WINDOWSTASKBAR_H
#define WINDOWSTASKBAR_H
#include <qtdockprovider.h>
#include <QWidget>

class JumpListsMenuExporter;
class WindowsTaskBar : public QtDockProvider
{
	Q_OBJECT
public:
	explicit WindowsTaskBar(QObject *parent = 0);
	virtual ~WindowsTaskBar();
	virtual bool isUsable() const;
	virtual void setMenu(QMenu *menu);
	virtual void setBadge(const QString &badge);
	virtual void setProgress(int percents);
	virtual void alert(bool on);
	virtual QVariant platformInvoke(const QByteArray &method, const QVariant &arguments);
	QWidget *window() const;
protected:
	QPixmap createBadge(const QString &badge) const;
	QSize overlayIconSize() const;
private:
	JumpListsMenuExporter *m_menuExporter;
	QWeakPointer<QWidget> m_widget;
private slots:
	void onAboutToQuit();
};

#endif // WINDOWSTASKBAR_H
