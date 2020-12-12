//
// Created by yz on 19-5-26.
//
#include <stdlib.h>
#include "pump_event/ev_def.h"
#include "pump_event/event_engine.h"

namespace Pump
{
namespace Event
{

CEventEngine::CEventEngine()
    : m_objEvCollector(*this)
    , m_objEvRegister(this)
    , m_objEvListenerMgr(this)
    , m_objEvReactor(this)
    , m_tid(-1)
{
}

CEventEngine::~CEventEngine()
{
}

int CEventEngine::routine()
{
    // 1.处理新注册的事件
    if (m_objEvRegister.register_events() != 0)
    {
        // TODO: 注册新事件错误
    }
    // 2.处理处于监听的事件，调用poll\select等
    if (m_objEvListenerMgr.listen_event() != 0)
    {
        // TODO: 监听条件事件错误
    }
    // 3.处理激活事件
    if (m_objEvReactor.react_events() != 0)
    {
        // TODO: 响应事件错误
    }
    return 0;
}

void CEventEngine::bind_thread(pump_thread_id tid)
{
    if (m_tid != -1)
    {
        assert(1 == 0);
    }
    m_tid = tid;
}

pump_thread_id CEventEngine::get_tid() const
{
    return m_tid;
}

int CEventEngine::insert_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        // TODO : err 参数错误
        return -1;
    }
    return m_objEvCollector.insert_newly_event(pEv);
}

int CEventEngine::remove_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        // TODO : err 参数错误
        return -1;
    }

    if (pEv->ONETIME)
    {
        // TODO : err 无法销毁一次性事件
        return -1;
    }

    switch (pEv->m_emEvCondition)
    {
    case CEvent::NONCONDITIONAL:
        return m_objEvCollector.remove_actived_event(pEv);
    case CEvent::CONDITIONAL:
        return m_objEvCollector.remove_listened_event(pEv);
    default:
        //TODO: err 指针参数指向的内存可能被破坏
        return -1;
    }
}

int CEventEngine::insert_listener(CEventListener *pListener)
{
    if (pListener == NULL)
    {
        // TODO : err 参数错误
        return -1;
    }
    return m_objEvListenerMgr.insert_event_listener(pListener);
}

int CEventEngine::remove_listener(CEventListener *pListener)
{
    if (pListener == NULL)
    {
        // TODO : err 参数错误
        return -1;
    }
    return m_objEvListenerMgr.remove_event_listener(pListener);
}

CEventListener *CEventEngine::get_listener_by_uuid_locked(const char *szUuid)
{
    if (szUuid == NULL)
    {
        //TODO: err 参数错误
        return NULL;
    }
    return m_objEvListenerMgr.get_listener_by_uuid_locked(szUuid);
}

void CEventEngine::unlock_listener_by_uuid(const char *szUuid)
{
    if (szUuid == NULL)
    {
        //TODO: err 参数错误
        return;
    }
    m_objEvListenerMgr.unlock_listener_by_uuid(szUuid);
}

CEventCollector &CEventEngine::get_event_collector()
{
    return m_objEvCollector;
}

}
}