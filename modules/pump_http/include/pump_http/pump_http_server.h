#ifndef PUMP_HTTP_SERVER_H
#define PUMP_HTTP_SERVER_H

#include "pump_macro/pump_pre.h"
#include <vector>
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_statemachine.h"
#include "pump_event/ev_def.h"
#include "pump_event/handle_event/handle_event_handler.h"
#include "pump_event/handle_event/handle_cap.h"
#include "pump_http/pump_http_config.h"
#include "pump_http/pump_http_request.h"

namespace Pump
{
namespace Http 
{

class CHttpSessionState
    : public ::Pump::Core::CStateMachineBase
{
public:
    enum HTTP_PROC_STATE
    {
        HTTP_PROC_HEAD = 0,
        HTTP_PROC_PAYLOAD,
    };

    enum HTTP_SESSION_STATE
    {
        HTTP_SESSION_DISCONNECTED = 0, ///< 未连接 
        HTTP_SESSION_CONNECTED, ///< 已连接
    };

    CHttpSessionState();
    ~CHttpSessionState();

    void ReSetSessionState(); //TODO 20200921 实现
    void SetSessionState(unsigned int dwSessionState);
    unsigned int GetSessionState() const;
    unsigned int GetSessionStateLocked();

private:
    unsigned int m_dwSessionState;
};

class PUMP_HTTP_CLASS CHttpSession
{
public:
    explicit CHttpSession(::Pump::Event::CEventLoop * pEventLoop);
    virtual ~CHttpSession();
    pump_int32_t ProcessRecvEvent(::Pump::Memory::IoBuffer & objBuffer);
    pump_int32_t ProcessAcceptEvent();
    ::Pump::Event::CEventLoop * GetBindingEventLoop();
private:
    pump_int32_t __ProcessRecvRawData(::Pump::Memory::IoBuffer & objBuffer);
    pump_int32_t  __PostHttpBodyProcessWork();
    static pump_pvoid_t PUMP_CALLBACK __ProcessHttpBody(pump_pvoid_t pData);
private:
    CHttpSessionState m_objHttpSessionState;
    CHttpRequest m_objHttpReqeust;
    ::Pump::Event::CEventLoop * m_pEventLoop;
    //CHttpConnectInfo m_objConnectInfo;
    //ACE_Mutex m_csMqttSession;
};

class PUMP_HTTP_CLASS CHttpServerRecvHandler
    : public ::Pump::Event::HandleEvent::CTCPSockRecvHandler
{
public:
    explicit CHttpServerRecvHandler(CHttpSession * pHttpSession);
    virtual ~CHttpServerRecvHandler();
    virtual pump_int32_t Open(pump_pvoid_t pParam);
    virtual pump_int32_t Close(pump_pvoid_t pParam);
    virtual pump_int32_t OnRecv(void *);
private:
    CHttpSession * m_pHttpSession;
};

class PUMP_HTTP_CLASS CHttpServerSendHandler
    : public ::Pump::Event::HandleEvent::CTCPSockSendHandler
{
public:
    explicit CHttpServerSendHandler(CHttpSession * pHttpSession);
    virtual ~CHttpServerSendHandler();
    virtual pump_int32_t Open(pump_pvoid_t pParam);
    virtual pump_int32_t Close(pump_pvoid_t pParam);
    virtual pump_int32_t OnSend(void *);
private:
    CHttpSession * m_pHttpSession;
};

class PUMP_HTTP_CLASS CHttpServerAcceptHandler
    : public ::Pump::Event::HandleEvent::CTCPSockAcceptHandler
{
public:
    CHttpServerAcceptHandler();
    virtual ~CHttpServerAcceptHandler();
    virtual pump_int32_t OnAccept(::Pump::Core::Net::CSock * pSock, ::Pump::Event::HandleEvent::CHandleCap * handleCap, pump_pvoid_t pData);
private:
    std::vector<CHttpServerRecvHandler*> m_vecRecvHandler;
};

}
}

#endif // PUMP_HTTP_SERVER_H