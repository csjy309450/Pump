#ifndef PUMP_EVENT_HANDLE_EVENT_HANDLER
#define PUMP_EVENT_HANDLE_EVENT_HANDLER
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_sock.h"
#include "pump_event/pump_event_config.h"
#include "pump_function/pump_function.hpp"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

class PUMP_EVENT_CLASS CTCPSockAccept;
class PUMP_EVENT_CLASS CTCPSockRecvHandler;
class PUMP_EVENT_CLASS CTCPSockSendHandler;

class PUMP_EVENT_CLASS CHandleEventHandler
    : public ::Pump::Event::CEventHandler
{
public:
    CHandleEventHandler();
    CHandleEventHandler(void* pData);
    virtual ~CHandleEventHandler();
};

class PUMP_EVENT_CLASS CTCPSockAcceptHandler
    : public CHandleEventHandler
{
public:
    class CTCPSockAcceptArgs
    {
    public:
        pump_char_t szIp_[PUMP_MAX_IP_ADDRLEN];
        pump_uint16_t sPort;
        pump_int32_t iBackLog_;
        CTCPSockAcceptArgs() {
            memset(szIp_, 0, sizeof(szIp_));
            sPort = -1;
            iBackLog_ = 5;
        }
    };
public:
    CTCPSockAcceptHandler();
    virtual ~CTCPSockAcceptHandler();
    virtual pump_int32_t Open(pump_pvoid_t pParam);
    virtual pump_int32_t Close(pump_pvoid_t pParam);
    virtual pump_int32_t OnAccept(::Pump::Core::Net::CSock * pSock, ::Pump::Event::HandleEvent::CHandleCap * handleCap, pump_pvoid_t pData);
protected:
    ::Pump::Core::Net::CSock * m_pListenSock;
    CTCPSockAccept * m_pEvTcpSockAccept;
    //CTCPSockRecvHandler m_objTcpRecvHandler;
    //CTCPSockSendHandler m_objTcpSendHandler;
};

class PUMP_EVENT_CLASS CTCPSockRecvHandler
    : public CHandleEventHandler
{
public:
    class CTCPSockRecvHandlerArgs
    {
    public:
        ::Pump::Core::Net::CSock * m_pSock;
        ::Pump::Event::HandleEvent::CHandleCap * m_handleCap;
        pump_pvoid_t m_pData;
    };
public:
    CTCPSockRecvHandler();
    virtual ~CTCPSockRecvHandler();
    virtual pump_int32_t Open(pump_pvoid_t pParam);
    virtual pump_int32_t Close(pump_pvoid_t pParam);
    virtual pump_int32_t OnRecv(pump_pvoid_t pData);
};

class PUMP_EVENT_CLASS CTCPSockSendHandler
    : public CHandleEventHandler
{
public:
    CTCPSockSendHandler();
    virtual ~CTCPSockSendHandler();
    virtual pump_int32_t Open(pump_pvoid_t pParam);
    virtual pump_int32_t Close(pump_pvoid_t pParam);
    virtual pump_int32_t OnSend();
};

}
}
}

#endif // PUMP_EVENT_HANDLE_EVENT_HANDLER
