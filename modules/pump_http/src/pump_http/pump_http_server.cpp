#include "pump_core/pump_core_logger.h"
#include "pump_core/pump_core_thread_pool.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_core/pump_core_string.h"
#include "pump_core/pump_core_statemachine.h"
#include "pump_event/handle_event/socket_event.h"
#include "pump_http/pump_http_server.h"
#include "pump_http/__pump_http_global_ctrl.h"

namespace Pump
{
namespace Http
{

CHttpSessionState::CHttpSessionState()
    : CStateMachineBase()
    , m_dwSessionState(PUMP_CORE_STATE_DEFAULT)
{

}

CHttpSessionState::~CHttpSessionState()
{

}

void CHttpSessionState::ReSetSessionState()
{
    ::Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    m_dwSessionState = PUMP_CORE_STATE_DEFAULT;
}

void CHttpSessionState::SetSessionState(unsigned int dwSessionState)
{
    ::Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    m_dwSessionState = dwSessionState;
}

unsigned int CHttpSessionState::GetSessionState() const
{
    return m_dwSessionState;
}

unsigned int CHttpSessionState::GetSessionStateLocked()
{
    m_csThis.Lock();
    return m_dwSessionState;
}

CHttpSession::CHttpSession(::Pump::Event::CEventLoop * pEventLoop)
    : m_pEventLoop(pEventLoop)
{

}

CHttpSession::~CHttpSession() {}

pump_int32_t CHttpSession::ProcessAcceptEvent()
{
    switch (m_objHttpSessionState.GetSessionState())
    {
    case CHttpSessionState::HTTP_SESSION_CONNECTED:
    {
    }
    break;
    case CHttpSessionState::HTTP_SESSION_DISCONNECTED:
    case PUMP_CORE_STATE_DEFAULT:
    {
        m_objHttpSessionState.SetSessionState(CHttpSessionState::HTTP_SESSION_CONNECTED);
    } break;
    }
    return 0;
}

pump_int32_t CHttpSession::ProcessRecvEvent(::Pump::Memory::IoBuffer & objBuffer)
{
    switch (m_objHttpSessionState.GetSessionState())
    {
    case CHttpSessionState::HTTP_SESSION_CONNECTED:
    {
        switch (m_objHttpSessionState.GetState())
        {
        case CHttpSessionState::HTTP_PROC_HEAD:
        case PUMP_CORE_STATE_DEFAULT:
        {
            /**processing http header*/
            if (m_objHttpReqeust.parse_header(objBuffer)==PUMP_FALSE)
            {
                break;
            }
            m_objHttpSessionState.SetState(CHttpSessionState::HTTP_PROC_PAYLOAD);
        } // don't break here!
        case CHttpSessionState::HTTP_PROC_PAYLOAD:
        {
            /**processing http body*/
            if (__ProcessRecvRawData(objBuffer)==PUMP_OK)
            {
                m_objHttpSessionState.SetState(CHttpSessionState::HTTP_PROC_HEAD);
            }
        } break;
        default:
            break;
        }
    }
    break;
    case CHttpSessionState::HTTP_SESSION_DISCONNECTED:
        break;
    }
    return 0;
}

::Pump::Event::CEventLoop * CHttpSession::GetBindingEventLoop()
{
    return m_pEventLoop;
}

pump_int32_t CHttpSession::__ProcessRecvRawData(::Pump::Memory::IoBuffer & objBuffer)
{
    if (m_objHttpReqeust.content_length()==0)
    {
        return PUMP_OK;
    }
    if (m_objHttpReqeust.content_length()>objBuffer.size())
    {
        // 数据没收完
        return PUMP_ERROR;
    }
    /** Processing data. */
    __PostHttpBodyProcessWork();
    //if (objBuffer.)
    //{
    //}
    return PUMP_OK;
}

pump_int32_t  CHttpSession::__PostHttpBodyProcessWork()
{
    __CPumpHttpGlobalCtrl * pGlobalCtrl = __CPumpHttpGlobalCtrl::GetGlobalCtrlLocked();
    if (!pGlobalCtrl)
    {
        __CPumpHttpGlobalCtrl::GlobalCtrlUnlocked();
        return PUMP_ERROR;
    }
    pump_handle_t hThreadPool = pGlobalCtrl->GetWorkingThxPoolLocked();
    if (hThreadPool==PUMP_INVALID_THREADPOOL)
    {
        pGlobalCtrl->WorkingThxPoolUnlock();
        __CPumpHttpGlobalCtrl::GlobalCtrlUnlocked();
        return PUMP_ERROR;
    }
    // TODO 20200926 这块最好换成待任务队列的工作线程池,保证Session处理的时序性
    // FIXME 20201010 因为使用了线程池，数据处理和接收异步，需要保证数据处理完成前，Session不被释放，否则导致崩溃
    pump_int32_t ret = PUMP_CORE_ThreadPool_Work(hThreadPool, CHttpSession::__ProcessHttpBody, this);
    pGlobalCtrl->WorkingThxPoolUnlock();
    __CPumpHttpGlobalCtrl::GlobalCtrlUnlocked();
    return PUMP_OK;
}

pump_pvoid_t PUMP_CALLBACK CHttpSession::__ProcessHttpBody(void * pData)
{
    PUMP_CORE_INFO("CHttpSession::__ProcessHttpBody()");
    if (!pData)
    {
        return NULL;
    }
    CHttpSession *pSession = static_cast<CHttpSession *>(pData);
    const char * pdata = pSession->m_objHttpReqeust.headers()["Content-Length"].value();
    //CHttpServerSendHandler objSend(pSession);
    //objSend.SetEventLoop(pSession->GetBindingEventLoop());
    //objSend.Open(NULL);
    return 0;
}

CHttpServerRecvHandler::CHttpServerRecvHandler(CHttpSession * pHttpSession)
    : m_pHttpSession(pHttpSession){}

CHttpServerRecvHandler::~CHttpServerRecvHandler() {}

pump_int32_t CHttpServerRecvHandler::Open(pump_pvoid_t pParam)
{
    if (!pParam)
    {
        return PUMP_ERROR;
    }
    this->m_pHttpSession->ProcessAcceptEvent();
    // FXIME 上锁
    CTCPSockRecvHandlerArgs * pArgs = static_cast<CTCPSockRecvHandlerArgs*>(pParam);
    ::Pump::Event::HandleEvent::CTCPSockRecv * pEvHttpRecv = new ::Pump::Event::HandleEvent::CTCPSockRecv();
    if (!pEvHttpRecv)
    {
        return -1;
    }
    pEvHttpRecv->Bind(::Pump::Event::HandleEvent::CTCPSockRecv::OnRecvCbType(&::Pump::Event::HandleEvent::CTCPSockRecvHandler::OnRecv, this));
    pEvHttpRecv->m_pHandle = pArgs->m_pSock;
    pEvHttpRecv->m_fd = pArgs->m_pSock->GetHandle();
    pArgs->m_handleCap->insert(pEvHttpRecv);
    return PUMP_OK;
}

pump_int32_t CHttpServerRecvHandler::Close(pump_pvoid_t pParam)
{
    return PUMP_OK;
}

pump_int32_t CHttpServerRecvHandler::OnRecv(void * pData)
{
    PUMP_CORE_INFO("CHttpServerRecvHandler::OnRecv()");
    if (!m_pHttpSession)
    {
        return -1;
    }
    ::Pump::Event::HandleEvent::CTCPSockRecv * pEvent = static_cast<::Pump::Event::HandleEvent::CTCPSockRecv *>(pData);
    return m_pHttpSession->ProcessRecvEvent(pEvent->m_objIoBufRecv);
}

CHttpServerAcceptHandler::CHttpServerAcceptHandler() {}

CHttpServerAcceptHandler::~CHttpServerAcceptHandler() {}

pump_int32_t CHttpServerAcceptHandler::OnAccept(::Pump::Core::Net::CSock * pSock, ::Pump::Event::HandleEvent::CHandleCap * handleCap, pump_pvoid_t pData)
{
    PUMP_CORE_INFO("CHttpServerAcceptHandler::OnAccept");
    // FIXME 暂时用CHttpServerAcceptHandler创建CHttpSession
    CHttpSession * pHttpSession = new CHttpSession(this->GetEventLoop());
    if (!pHttpSession)
    {
        return -1;
    }
    // 允许读事件,投递读事件
    CHttpServerRecvHandler * pRecvHandler = new (std::nothrow)CHttpServerRecvHandler(pHttpSession);
    if (!pRecvHandler)
    {
        return  PUMP_ERROR;
    }
    CHttpServerRecvHandler::CTCPSockRecvHandlerArgs objArgs;
    objArgs.m_pSock = pSock;
    objArgs.m_handleCap = handleCap;
    objArgs.m_pData = NULL;
    pRecvHandler->Open(&objArgs);
    m_vecRecvHandler.push_back(pRecvHandler);
    return PUMP_OK;
}

}
}