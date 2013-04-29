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
