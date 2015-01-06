/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Copyright (C) 2012-2014 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#include "screenmonitor.h"
#include <QtDebug>

static const struct SensEvents {
    const wchar_t* method;
    const wchar_t* uuid;
} sensEvents[] = {
    { L"DisplayLock",       L"{B7E2C510-501A-4961-938F-A458970930D7}" },
    { L"DisplayUnlock",     L"{11305987-8FFC-41AD-A264-991BD5B7488A}" },
    { L"StartScreenSaver",  L"{6E2D26DF-0095-4EC4-AE00-2395F09AF7F2}" },
    { L"StopScreenSaver",   L"{F53426BC-412F-41E8-9A5F-E5FA8A164BD6}" }
};

static const wchar_t* SENSGUID_EVENTCLASS_LOGON = L"{d5978630-5b9f-11d1-8dd2-00aa004abd5e}";

#ifdef Q_CC_MINGW
const GUID IID_ISensLogon = {0xd597bab3, 0x5b9f, 0x11d1, {0x8d,0xd2,0x00,0xaa,0x00,0x4a,0xbd,0x5e}};
const GUID IID_IEventSystem = {0x4E14FB9F, 0x2E22, 0x11D1, {0x99,0x64,0x00,0xC0,0x4F,0xBB,0xB3,0x45}};
const GUID CLSID_CEventSystem = {0x4E14FBA2, 0x2E22, 0x11D1, {0x99,0x64,0x00,0xC0,0x4F,0xBB,0xB3,0x45}};
const GUID IID_IEventSubscription = {0x4A6B0E15, 0x2E38, 0x11D1, {0x99,0x65,0x00,0xC0,0x4F,0xBB,0xB3,0x45}};
const GUID CLSID_CEventSubscription = {0x7542e960, 0x79c7, 0x11d1, {0x88,0xf9,0x00,0x80,0xc7,0xd7,0x71,0xbf}};
#endif

#define BSTR_CALL(Method, Arg) \
    bstr = SysAllocString(Arg); \
    hr = Method; \
    SysFreeString(bstr);

ScreenMonitor::ScreenMonitor() : refcount(0), eventSystem(NULL)
{
    HRESULT hr = CoInitialize(NULL);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_CEventSystem, NULL, CLSCTX_SERVER,
                              IID_IEventSystem, (LPVOID*)&eventSystem);
        if (SUCCEEDED(hr)) {
            eventSystem->AddRef();
            BSTR bstr = NULL;
            IEventSubscription* subscription = NULL;
            for (int i = 0; i < 4; ++i) {
                hr = CoCreateInstance(CLSID_CEventSubscription, NULL, CLSCTX_SERVER,
                                      IID_IEventSubscription, (LPVOID*)&subscription);
                if (FAILED(hr))
                    continue;

                BSTR_CALL(subscription->put_EventClassID(bstr), SENSGUID_EVENTCLASS_LOGON)
                if (FAILED(hr))
                    continue;

                hr = subscription->put_SubscriberInterface((IUnknown*)this);
                if (FAILED(hr))
                    continue;

                BSTR_CALL(subscription->put_MethodName(bstr), sensEvents[i].method)
                if (FAILED(hr))
                    continue;

                BSTR_CALL(subscription->put_SubscriptionName(bstr), sensEvents[i].method)
                if (FAILED(hr))
                    continue;

                BSTR_CALL(subscription->put_SubscriptionID(bstr), sensEvents[i].uuid)
                if (FAILED(hr))
                    continue;

                hr = subscription->put_PerUser(TRUE);
                if (FAILED(hr))
                    continue;

                BSTR_CALL(eventSystem->Store(bstr, (IUnknown*)subscription), PROGID_EventSubscription)
                if (FAILED(hr))
                    break;

                subscription->Release();
                subscription = NULL;
            }
        }
    }
    if (FAILED(hr))
        qWarning() << "ScreenMonitor: COM failure:" << GetLastError();
}

ScreenMonitor::~ScreenMonitor()
{
    if (eventSystem)
        eventSystem->Release();
    CoUninitialize();
}

ULONG STDMETHODCALLTYPE ScreenMonitor::AddRef()
{
    return ++refcount;
}

ULONG STDMETHODCALLTYPE ScreenMonitor::Release()
{
    return --refcount;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::GetTypeInfo(UINT, LCID, ITypeInfo**)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::GetTypeInfoCount(UINT*)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::QueryInterface(REFIID riid, void** obj)
{
    if (IsEqualIID(riid, IID_IUnknown))
        *obj = (IUnknown*)this;
    else if (IsEqualIID(riid, IID_ISensLogon))
        *obj = (ISensLogon*)this;
    else
        return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::DisplayLock(BSTR userName)
{
    Q_UNUSED(userName);
    emit screenLocked();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::DisplayUnlock(BSTR userName)
{
    Q_UNUSED(userName);
    emit screenUnlocked();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::Logoff(BSTR userName)
{
    Q_UNUSED(userName);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::Logon(BSTR userName)
{
    Q_UNUSED(userName);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::StartScreenSaver(BSTR userName)
{
    Q_UNUSED(userName);
    emit screenSaverStarted();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::StartShell(BSTR userName)
{
    Q_UNUSED(userName);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ScreenMonitor::StopScreenSaver(BSTR userName)
{
    Q_UNUSED(userName);
    emit screenSaverStopped();
    return S_OK;
}
