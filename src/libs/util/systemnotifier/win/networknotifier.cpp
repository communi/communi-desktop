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

#include "networknotifier.h"
#include <QtDebug>

NetworkNotifier::NetworkNotifier()
    : cookie(0), cp(NULL), manager(NULL), container(NULL)
{
    HRESULT hr = CoInitialize(NULL);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL,
                              IID_INetworkListManager, (LPVOID*)&manager);
        if (SUCCEEDED(hr)) {
            manager->AddRef();
            hr = manager->QueryInterface(IID_IConnectionPointContainer, (void**)&container);
            if (SUCCEEDED(hr)) {
                container->AddRef();
                hr = container->FindConnectionPoint(IID_INetworkListManagerEvents, &cp);
                if (SUCCEEDED(hr)) {
                    cp->AddRef();
                    hr = cp->Advise((IUnknown*)this, &cookie);
                }
            }
        }
    }
    if (FAILED(hr))
        qWarning() << "NetworkNotifier: COM failure:" << GetLastError();
}

NetworkNotifier::~NetworkNotifier()
{
    if (cp) {
        if (cookie)
            cp->Unadvise(cookie);
        cp->Release();
    }
    if (container)
        container->Release();
    if (manager)
        manager->Release();
    CoUninitialize();
}

ULONG STDMETHODCALLTYPE NetworkNotifier::AddRef()
{
    return -1;
}

ULONG STDMETHODCALLTYPE NetworkNotifier::Release()
{
    return -1;
}

HRESULT STDMETHODCALLTYPE NetworkNotifier::QueryInterface(REFIID riid, void** obj)
{
    if (IsEqualIID(riid, IID_IUnknown))
        *obj = (IUnknown*)this;
    else if (IsEqualIID(riid ,IID_INetworkListManagerEvents))
        *obj = (INetworkListManagerEvents*)this;
    else
        return E_NOINTERFACE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE NetworkNotifier::ConnectivityChanged(NLM_CONNECTIVITY connectivity)
{
    if (connectivity == NLM_CONNECTIVITY_DISCONNECTED)
        emit offline();
    else if (connectivity & NLM_CONNECTIVITY_IPV4_INTERNET ||
             connectivity & NLM_CONNECTIVITY_IPV4_INTERNET)
        emit online();
    return S_OK;
}
