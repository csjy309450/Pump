//
// Created by yz on 19-5-26.
//
#include "pump_macro/pump_pre.h"
#ifdef __linux__
#include <unistd.h>
#include <pthread.h>
#elif (defined _WIN32)
#include <synchapi.h>
#endif // __linux__
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_api.h"
#include "pump_event/event_interface.h"
#include "pump_event/event_loop.h"

namespace Pump
{
namespace Event
{

CEventLoop::CEventLoop()
    : ::Pump::Core::Thread::CThread(this)
    , m_dwState(LOOPSTATE_STOP) {}

CEventLoop::~CEventLoop()
{
    Stop();
}

pump_int32_t CEventLoop::Start()
{
    ::Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    if (m_objEvEngine.get_tid() != PUMP_INVALID_THREAD
        || this->GetTID() == m_objEvEngine.get_tid())
    {
        // error 事件循环已经启动
        return -1;
    }
    // 1 启动事件循环业务线程
    pump_int32_t ret = ::Pump::Core::Thread::CThread::Start();
    if (ret == PUMP_ERROR)
    {
        return PUMP_ERROR;
    }
    // 2 给事件引擎绑定TID
    m_objEvEngine.bind_thread(this->GetTID());
    // 3 向__CEventHub注册事件收集器
    register_event_collector(&m_objEvEngine.get_event_collector());
    return PUMP_OK;
}

pump_int32_t CEventLoop::Stop()
{
    ::Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    if (m_objEvEngine.get_tid() == PUMP_INVALID_THREAD)
    {
        // error 事件循环未启动
        return PUMP_ERROR;
    }
    SetState(LOOPSTATE_STOP);
    // 1 终止业务线程
    pump_int32_t ret = ::Pump::Core::Thread::CThread::Stop();
    // 2 从__CEventHub中注销事件收集器
    unregister_event_collector_by_tid(m_objEvEngine.get_tid());
    return PUMP_OK;
}

pump_int32_t CEventLoop::Pause()
{
    ::Pump::Core::Thread::CMutexGuilder locker(m_csThis);
    SetState(LOOPSTATE_PAUSE);
    return 0;
}

pump_int32_t CEventLoop::Resume()
{
    if (m_dwState == LOOPSTATE_PAUSE)
    {
        ::Pump::Core::Thread::CMutexGuilder locker(m_csThis);
        m_dwState = LOOPSTATE_START;
    }
    return 0;
}

pump_bool_t CEventLoop::IsStop()
{
    if (m_dwState == LOOPSTATE_STOP)
    {
        return true;
    }
    return false;
}

pump_int32_t CEventLoop::InsertEvent(pump_ev_handle hEv)
{
    if (PUMP_EVENT_IS_HANDLE_AVAIL(hEv))
    {
        //TODO: 无效句柄
        return -1;
    }
    return m_objEvEngine.insert_event((CEvent *)hEv);
}

pump_int32_t CEventLoop::RemoveEvent(pump_ev_handle hEv)
{
    if (PUMP_EVENT_IS_HANDLE_AVAIL(hEv))
    {
        //TODO: 无效句柄
        return -1;
    }
    return m_objEvEngine.remove_event((CEvent *) hEv);
}

pump_int32_t CEventLoop::InsertListener(pump_listener_handle hListener)
{
    if (PUMP_EVENT_IS_HANDLE_AVAIL(hListener))
    {
        //TODO: 无效句柄
        return -1;
    }
    return m_objEvEngine.insert_listener((CEventListener *) hListener);
}

int CEventLoop::RemoveListener(pump_listener_handle hListener)
{
    if (PUMP_EVENT_IS_HANDLE_AVAIL(hListener))
    {
        //TODO: 无效句柄
        return -1;
    }
    return m_objEvEngine.remove_listener((CEventListener *) hListener);
}

CEventCollector &CEventLoop::GetEventCollector()
{
    return m_objEvEngine.get_event_collector();
}

//CEventEngine &CEventLoop::get_event_engine()
//{
//    return m_objEvEngine;
//}

//pump_thread_id CEventLoop::GetTID() const
//{
//    return m_objEvEngine.get_tid();
//}

pump_void_t CEventLoop::SetState(LoopState emState)
{
    m_dwState = emState;
}

pump_void_t * CEventLoop::ThreadCallback(pump_void_t * pData)
{
    if (pData == NULL)
    {
        //TODO: err 参数错误,线程启动失败
        return NULL;
    }
    CEventLoop *pLoop = (CEventLoop *)pData;
    pLoop->SetState(LOOPSTATE_START);
    while (!pLoop->IsStop())
    {
        if (pLoop->m_dwState == LOOPSTATE_PAUSE)
        {
            PUMP_CORE_Sleep(500);
            continue;
        }
#ifdef _TEST_LEVEL_DEBUG
        PUMP_CORE_INFO << "**** pump_ev loop batch ****";
#endif //_TEST_LEVEL_DEBUG
        PUMP_CORE_Sleep(500);
        pLoop->m_objEvEngine.routine();
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "**** pump_ev loop ending ****";
#endif //_TEST_LEVEL_DEBUG
    return NULL;
}

}
}