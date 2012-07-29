/****************************************************************************
 *  jumplistsmenuexporter_p.h
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

#ifndef JUMPLISTSMENUEXPORTER_P_H
#define JUMPLISTSMENUEXPORTER_P_H
#include "jumplistsmenuexporter.h"
#include <QWeakPointer>
#include <QMenu>
#include <QApplication>
#include "wrapper/taskbar.h"
#include "temporaryicon.h"
#include <QDebug>
#include <QUuid>

struct Data;
typedef QVector<ActionInfo> ActionInfoList;
typedef QVector<wchar_t> WCharArray;

static WCharArray toWCharArray(const QString &str)
{
	WCharArray array(str.length() + 1);
	str.toWCharArray(array.data());
	return array;
}

struct Data
{
	Data(QAction *action) : action(action), icon(action->icon()),
		id(QUuid::createUuid().toString().toAscii()),
		name(toWCharArray(action->text())),
		description(toWCharArray(action->toolTip())),
		iconPath(toWCharArray(icon.filePath()))
	{
	}
	QWeakPointer<QAction> action;
	TemporaryIcon icon;
	QByteArray id;
	WCharArray name;
	WCharArray description;
	WCharArray iconPath;
};

void invokeQAction(void *pointer)
{
	Data *data = reinterpret_cast<Data*>(pointer);
	if (data->action) {
		qDebug() << data->action.data();
		data->action.data()->trigger();
	}
}

class JumpListsMenuExporter;
class JumpListsMenuExporterPrivate
{
	Q_DECLARE_PUBLIC(JumpListsMenuExporter)
public:
	JumpListsMenuExporterPrivate(JumpListsMenuExporter *q) : q_ptr(q)
	{
		setAppId(qAppName());
		setActionInvoker(invokeQAction);
	}
	~JumpListsMenuExporterPrivate()
	{
	}
	ActionInfoList serialize(QMenu *menu);
	ActionInfo serialize(QAction *action);
	QSize pixmapSize() const;
	void setAppId(const QString &id);

	JumpListsMenuExporter *q_ptr;
	void updateJumpLists();
	QWeakPointer<QMenu> menu;
	ActionInfoList actionInfoList;
	WCharArray appId;
};


#endif // JUMPLISTSMENUEXPORTER_P_H
