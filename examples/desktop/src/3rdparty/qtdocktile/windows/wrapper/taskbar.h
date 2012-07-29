/****************************************************************************
 *  taskbar.h
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

#ifndef TASKBAR_H
#define TASKBAR_H
#include <windows.h>

#ifdef TASKWRAPPER
#	define EXPORT   extern "C" __declspec(dllexport)
#else
#	define EXPORT   extern "C"
#endif

enum ProgressState
{
	ProgressStateNormal,
	ProgressStatePaused,
	ProgressStateError,
	ProgressStateIndeterminate,
	ProgressStateNone
};

enum ActionType
{
	ActionTypeNormal,
	ActionTypeSeparator
};

struct ActionInfo
{
	const char *id;
	wchar_t *name;
	wchar_t *description;
	wchar_t *iconPath;
	ActionType type;
	void *data;
};

typedef void (*ActionInvoker)(void*);

EXPORT void setActionInvoker(ActionInvoker pointer);
EXPORT void setJumpLists(ActionInfo *list, size_t size);
EXPORT void deleteJumpLists();

EXPORT void setApplicationId(const wchar_t *appId);
EXPORT void setOverlayIcon(HWND winId, HICON icon, wchar_t *description = 0);
EXPORT void clearOverlayIcon(HWND winId);
EXPORT void setProgressValue(HWND winId, int percents);
EXPORT void setProgressState(HWND winId, ProgressState state);

#endif // TASKBAR_H
