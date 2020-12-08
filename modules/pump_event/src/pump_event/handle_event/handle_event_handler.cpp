#include "pump_event/event_loop.h"
#include "pump_event/handle_event/handle_event_listener.h"
#include "pump_event/handle_event/socket_event.h"
#include "pump_event/event_handler.h"
#include "pump_event/handle_event/handle_event_handler.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

CHandleEventHandler::CHandleEventHandler()
    : ::Pump::Event::CEventHandler()
{
}

CHandleEventHandler::CHandleEventHandler(void* pData)
    : ::Pump::Event::CEventHandler(pData)
{
}

CHandleEventHandler::~CHandleEventHandler()
{
}

CTCPSockAcceptHandler::CTCPSockAcceptHandler()
{
    
}

CTCPSockAcceptHandler::~CTCPSockAcceptHandler()
{
    
}

pump_int32_t CTCPSockAcceptHandler::Open(pump_pvoid_t pParam)
{
    //if (!pParam)
    //{
    //    return PUMP_ERROR;
    //}
    CTCPSockAcceptArgs * pArgs = static_cast<CTCPSockAcceptArgs *>(pParam);
    m_pListenSock = ::Pump::Core::Net::CSock::CreateSock(PUMP_AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!m_pListenSock)
    {
        return PUMP_ERROR;
    }
    CHandleInsert * pEvHInsert = new CHandleInsert(m_pListenSock);
    m_pLoop->InsertEvent((pump_ev_handle)pEvHInsert);
    delete pEvHInsert;
    m_pEvTcpSockAccept = new CTCPSockAccept(m_pListenSock);
    m_pEvTcpSockAccept->Bind(CTCPSockAccept::OnAcceptCbType(&CTCPSockAcceptHandler::OnAccept, this));
    CEvent::EvData pData;
    pData.pData_ = (void*)m_pLoop->GetTID();
    pData.bDel_ = false;
    m_pEvTcpSockAccept->set_data(Pump::Event::EVSTATE_DESTROYED, pData);
    m_pEvTcpSockAccept->m_struArg.iAf_ = PUMP_AF_INET;
    //FIXME 20200926 改成可配置的
    strncpy(m_pEvTcpSockAccept->m_struArg.szIp_, pArgs->szIp_, strlen(pArgs->szIp_));
    m_pEvTcpSockAccept->m_struArg.sPort = pArgs->sPort;
    m_pEvTcpSockAccept->m_struArg.iBackLog_ = pArgs->iBackLog_;
    m_pLoop->InsertEvent((pump_ev_handle)m_pEvTcpSockAccept);
    return PUMP_OK;
}

pump_int32_t CTCPSockAcceptHandler::Close(pump_pvoid_t pParam)
{
    m_pListenSock->Close();
    CHandleRemove * pEvSoClose = new CHandleRemove();
    pEvSoClose->m_pHandle = m_pListenSock;
    m_pLoop->InsertEvent((pump_ev_handle)pEvSoClose);
    delete pEvSoClose;
    delete m_pEvTcpSockAccept;
    m_pEvTcpSockAccept = PUMP_NULL;
    return PUMP_OK;
}

pump_int32_t CTCPSockAcceptHandler::OnAccept(::Pump::Core::Net::CSock * pSock, CHandleCap * handleCap, pump_pvoid_t pData)
{
    PUMP_CORE_INFO << "CTCPSockAcceptHandler::OnAccept";
    // 允许读事件,投递读事件
    CTCPSockRecv *pSockRecv = new CTCPSockRecv();
    pSockRecv->m_fd = pSock->GetHandle();
    pSockRecv->m_pHandle = pSock;
    handleCap->insert(pSockRecv);
    return PUMP_OK;
}

CTCPSockRecvHandler::CTCPSockRecvHandler()
{
    
}

CTCPSockRecvHandler::~CTCPSockRecvHandler()
{
    
}

pump_int32_t CTCPSockRecvHandler::Open(pump_pvoid_t pParam)
{
    return PUMP_OK;
}

pump_int32_t CTCPSockRecvHandler::Close(pump_pvoid_t pParam)
{
    return PUMP_OK;
}

pump_int32_t CTCPSockRecvHandler::OnRecv(pump_pvoid_t pData)
{
    return PUMP_OK;
}

}
}
}