/*
* Copyright (C) 2008-2013 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef NETWORKNOTIFIER_H
#define NETWORKNOTIFIER_H

#include <QObject>
#include <netlistmgr.h>

class NetworkNotifier : public QObject, public INetworkListManagerEvents
{
    Q_OBJECT

public:
    NetworkNotifier();
    ~NetworkNotifier();

    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** obj);
    HRESULT STDMETHODCALLTYPE ConnectivityChanged(NLM_CONNECTIVITY connectivity);

signals:
    void online();
    void offline();

private:
    DWORD cookie;
    IConnectionPoint *cp;
    INetworkListManager* manager;
    IConnectionPointContainer* container;
};

#endif // NETWORKNOTIFIER_H
