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
