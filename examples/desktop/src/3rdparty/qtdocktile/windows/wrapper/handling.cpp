/****************************************************************************
 *  handling.cpp
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

#include <process.h>
#include "handling.h"
#include "taskbar.h"

EXPORT void CALLBACK
RundllCallback(HWND hwnd, HINSTANCE hinst, LPSTR cmdLine, int cmdShow);

void CALLBACK RundllCallback(HWND, HINSTANCE, LPSTR cmdLine, int)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sk;
	sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sk == INVALID_SOCKET) {
		WSACleanup();
		return;
	}

	sockaddr_in sai;
	sai.sin_family      = AF_INET;
	sai.sin_addr.s_addr = inet_addr("127.0.0.1");
	sai.sin_port        = htons(Handler::port);

	if (connect(sk, reinterpret_cast<SOCKADDR*>(&sai), sizeof(sai)) == SOCKET_ERROR) {
		WSACleanup();
		return;
	}

	std::string cmd = cmdLine;
	send(sk, cmd.c_str(), cmd.size(), 0);
	closesocket(sk);
	WSACleanup();
}

Handler::Handler()
	: m_callback(0),
	  m_listenTread(0)
{
	m_listenTread = _beginthread(&Handler::listenner, 0, 0);
}

Handler::~Handler()
{
	CloseHandle(reinterpret_cast<HANDLE>(m_listenTread));
}

SOCKET Handler::makeSocket()
{
	WSADATA wsaData;
	SOCKET sock = 0;
	sockaddr_in sin;

	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != NO_ERROR) {
		return INVALID_SOCKET;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		WSACleanup();
		return INVALID_SOCKET;
	}

	sin.sin_family      = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port        = htons(Handler::port);

	res = bind(sock, (SOCKADDR *) &sin, sizeof(sin));
	if (res == SOCKET_ERROR) {
		closesocket(sock);
		WSACleanup();
		return INVALID_SOCKET;
	}

	if (listen(sock, 2) == SOCKET_ERROR) {
		closesocket(sock);
		WSACleanup();
		return INVALID_SOCKET;
	}
	return sock;
}

void Handler::listenner(void *)
{
	SOCKET connection, sock = makeSocket();
	if (INVALID_SOCKET == sock)
		return;

	const int buffLen = 512;
	char *buffer = new char[buffLen];
	int datalen = 0;
	while ((connection = accept(sock, 0, 0)) != INVALID_SOCKET) {
		int len = 0;
		while ((len = recv(connection, &buffer[datalen], buffLen-datalen, 0)) > 0) {
			datalen += len;
			Sleep(1);
		}
		buffer[datalen] = 0;
		if (instance()->m_callback)
			instance()->m_callback(buffer);
		datalen = 0;
	}
	delete[] buffer;
}
