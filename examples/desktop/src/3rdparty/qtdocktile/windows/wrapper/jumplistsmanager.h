/****************************************************************************
 *  jumplistsmanager.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <gorthauer87@ya.ru>
 *  Copyright (c) 2011 Ivan Vizir <define-true-false@yandex.com>
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

#ifndef JUMPLISTSMANAGER_H
#define JUMPLISTSMANAGER_H
#include "windows.h"
#include <string>
#include <map>

typedef void (*ActionInvoker)(void *data);
struct ActionInfo;
struct ICustomDestinationList;
struct IObjectCollection;

class JumpListsManager
{
public:
	static JumpListsManager *instance();
	void setAppId(const wchar_t *appId);
	//TODO
	void beginList();
	void addTask(ActionInfo *info);
	void deleteList(const wchar_t *appId = 0);
	void addSeparator();
	void beginCategory(const wchar_t *name);
	void commitList();
	void setActionInvoker(ActionInvoker pointer);
	ActionInvoker actionInvoker() const;
protected:
	JumpListsManager();
	~JumpListsManager();
private:
	const wchar_t *m_appId;
	ActionInvoker m_invoker;
	ICustomDestinationList *m_destList;
	IObjectCollection *m_destListContent;
	std::wstring m_wrapperPath;

	typedef std::map<std::string, ActionInfo*> ActionInfoMap;
	ActionInfoMap m_actionInfoMap;

	std::wstring makeArgs(ActionInfo *info);
	static void handlerCallback(const char *);
	static ActionInfo *actionInfo(const char *id);
};

inline JumpListsManager *jumpListsManager()
{
	return JumpListsManager::instance();
}

#endif // JUMPLISTSMANAGER_H
