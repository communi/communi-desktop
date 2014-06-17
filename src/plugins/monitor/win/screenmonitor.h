/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef SCREENMONITOR_H
#define SCREENMONITOR_H

#include <QObject>
#include <eventsys.h>
#include <sensevts.h>

class ScreenMonitor : public QObject, public ISensLogon
{
    Q_OBJECT

public:
    ScreenMonitor();
    ~ScreenMonitor();

    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**);
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT*);
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
    HRESULT STDMETHODCALLTYPE Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** obj);
    HRESULT STDMETHODCALLTYPE DisplayLock(BSTR userName);
    HRESULT STDMETHODCALLTYPE DisplayUnlock(BSTR userName);
    HRESULT STDMETHODCALLTYPE Logoff(BSTR userName);
    HRESULT STDMETHODCALLTYPE Logon(BSTR userName);
    HRESULT STDMETHODCALLTYPE StartScreenSaver(BSTR userName);
    HRESULT STDMETHODCALLTYPE StartShell(BSTR userName);
    HRESULT STDMETHODCALLTYPE StopScreenSaver(BSTR userName);

signals:
    void screenLocked();
    void screenUnlocked();
    void screenSaverStarted();
    void screenSaverStopped();

private:
    quint32 refcount;
    IEventSystem* eventSystem;
};

#endif // SCREENMONITOR_H
