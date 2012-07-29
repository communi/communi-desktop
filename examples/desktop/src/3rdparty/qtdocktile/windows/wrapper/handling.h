/****************************************************************************
 *  handling.h
 *
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

#ifndef HANDLING_H
#define HANDLING_H

#include <windows.h>
#include <string>

typedef void (*HandlerCallback)(const char*);

class Handler
{
public:
	static const int port = 42042;

	inline void setCallback(HandlerCallback cb)
	{
		m_callback = cb;
	}
	static Handler *instance()
	{
		static Handler h;
		return &h;
	}

private:
	HandlerCallback m_callback;
	UINT_PTR m_listenTread;

	Handler();
	~Handler();
	static SOCKET makeSocket();
	static void listenner(void *);
};

#endif // HANDLING_H
