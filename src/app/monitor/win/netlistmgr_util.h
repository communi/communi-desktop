#ifndef NETLISTMGR_UTIL_H
#define NETLISTMGR_UTIL_H

#include <netlistmgr.h>

#if defined(Q_CC_MINGW)
// for some reason MinGW ships an incomplete netlistmgr.h

#include <ocidl.h> // IConnectionPoint and IConnectionPointContainer

#ifdef __cplusplus
extern "C" {
#endif
static const GUID CLSID_NetworkListManager      = { 0xDCB00C01, 0x570F, 0x4A9B,{ 0x8D,0x69, 0x19,0x9F,0xDB,0xA5,0x72,0x3B } };
static const GUID IID_INetworkListManager       = { 0xDCB00000, 0x570F, 0x4A9B,{ 0x8D,0x69, 0x19,0x9F,0xDB,0xA5,0x72,0x3B } };
static const GUID IID_INetworkListManagerEvents = { 0xDCB00001, 0x570F, 0x4A9B,{ 0x8D,0x69, 0x19,0x9F,0xDB,0xA5,0x72,0x3B } };

interface INetworkListManagerEvents : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE ConnectivityChanged(
            NLM_CONNECTIVITY newConnectivity) = 0;
};

interface IEnumNetworks;
interface INetwork;
interface IEnumNetworkConnections;
interface INetworkConnection;

interface INetworkListManager : public IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE GetNetworks(
            NLM_ENUM_NETWORK Flags,
            IEnumNetworks **ppEnumNetwork) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNetwork(
            GUID gdNetworkId,
            INetwork **ppNetwork) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNetworkConnections(
            IEnumNetworkConnections **ppEnum) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetNetworkConnection(
            GUID gdNetworkConnectionId,
            INetworkConnection **ppNetworkConnection) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_IsConnectedToInternet(
            VARIANT_BOOL *pbIsConnected) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_IsConnected(
            VARIANT_BOOL *pbIsConnected) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetConnectivity(
            NLM_CONNECTIVITY *pConnectivity) = 0;
};

#ifdef __cplusplus
}
#endif

#endif // !(defined(Q_CC_MINGW)

#endif // NETLISTMGR_UTIL_H
