//
// Created by yz on 19-5-26.
//
#include <stdlib.h>
#include <algorithm>
#include "pump_event/event_interface.h"
#include "pump_event/event_listener_mgr.h"
#include "pump_event/event_engine.h"

namespace Pump
{
namespace Event
{

CEventListenerMgr::CEventListenerMgr(CEventEngine *pEvEngine)
    : m_pEvEngine(pEvEngine),
    m_refEvCollector(pEvEngine->get_event_collector())
{
    //  m_objEvListenerContainer.resize(g_maxEvListner);
}

CEventListenerMgr::~CEventListenerMgr()
{
}

int CEventListenerMgr::insert_event_listener(CEventListener *pEvListener)
{
    if (pEvListener == NULL)
    {
        //TODO: err 参数错误
        return -1;
    }
    CEventListenerMgr::CListenerItem item;
    item.m_pEvListener = pEvListener;
    m_csEvListenerContainer.writeLock();
    m_objEvListenerContainer.push_back(item);
    m_csEvListenerContainer.writeUnlock();
    return 0;
}

int CEventListenerMgr::remove_event_listener(CEventListener *pEvListener)
{
    if (pEvListener == NULL)
    {
        //TODO: err 参数错误
        return -1;
    }
    m_csEvListenerContainer.writeLock();
    CEventListenerMgr::IteratorType it = std::find_if(m_objEvListenerContainer.begin(),
        m_objEvListenerContainer.end(),
        CFindPredicate(pEvListener));
    if (it == m_objEvListenerContainer.end())
    {
        //TODO: err 未找到对应监听器
        return -1;
    }
    m_objEvListenerContainer.erase(it);
    m_csEvListenerContainer.writeUnlock();
    return 0;
}

CEventListener *CEventListenerMgr::get_listener_by_uuid_locked(const char *szUuid)
{
    if (szUuid == NULL)
    {
        //TODO: err 参数错误
        return NULL;
    }
    m_csEvListenerContainer.readLock();
    CEventListenerMgr::IteratorType it;
    for (it = m_objEvListenerContainer.begin();
        it != m_objEvListenerContainer.end(); it++)
    {
        if (!strcmp((*it).m_pEvListener->m_szUuid, szUuid))
        {
            break;
        }
    }
    if (it == m_objEvListenerContainer.end())
    {
        m_csEvListenerContainer.readLock();
        return NULL;
    }
    (*it).m_csItem.Lock();
    m_csEvListenerContainer.readLock();
    return (*it).m_pEvListener;
}

void CEventListenerMgr::unlock_listener_by_uuid(const char *szUuid)
{
    if (szUuid == NULL)
    {
        //TODO: err 参数错误
        return;
    }
    m_csEvListenerContainer.readLock();
    CEventListenerMgr::IteratorType it;
    for (it = m_objEvListenerContainer.begin();
        it != m_objEvListenerContainer.end(); it++)
    {
        if (!strcmp((*it).m_pEvListener->m_szUuid, szUuid))
        {
            break;
        }
    }
    if (it != m_objEvListenerContainer.end())
    {
        (*it).m_csItem.Unlock();
    }
    m_csEvListenerContainer.readUnlock();
    m_csEvListenerContainer.readUnlock();
}

//int CEventListenerMgr::insert_event(CEvent *__p_ev)
//{
//  if (__p_ev == NULL)
//  {
//    //TODO: err 参数错误
//    return -1;
//  }
//  //TODO: 锁住EventListener列表,主要是静止删除操作,待优化
//  m_csEvListenerContainer.readLock();
//  m_refEvCollector.get_listened_event_mgr().insert_event(__p_ev);
//  m_csEvListenerContainer.readUnlock();
//  return 0;
//}
//
//int CEventListenerMgr::remove_event(CEvent *__p_ev)
//{
//  if (__p_ev == NULL)
//  {
//    //TODO: err 参数错误
//    return -1;
//  }
//  //TODO: 锁住EventListener列表,主要是静止删除操作,待优化
//  m_csEvListenerContainer.readLock();
//  bool ret = m_refEvCollector.get_listened_event_mgr().remove_event(__p_ev);
//  if (ret)
//  {
//    CEvent::EvData data = __p_ev->m_evData[EVSTATE_DESTROYED];
//    pump_thread_id tid = (pump_thread_id) data.pData;
//    CEventEngine *pEngine = get_event_engine_by_tid_locked(tid);
//    if (pEngine == NULL)
//    {
//      //TODO: err 未找到绑定 CEventEngine
//      return -1;
//    } else
//    {
//      __p_ev->_on_destroyed(*pEngine, __p_ev->m_evData[EVSTATE_DESTROYED]);
//      unlock_event_engine_by_tid(tid);
//    }
//  }
//  m_csEvListenerContainer.readUnlock();
//  return (ret ? 1 : -1);
//}

int CEventListenerMgr::listen_event()
{
    m_csEvListenerContainer.readLock();
    for (CEventListenerMgr::IteratorType it = m_objEvListenerContainer.begin();
        it != m_objEvListenerContainer.end(); it++)
    {
        (*it).m_pEvListener->route();
    }
    m_csEvListenerContainer.readUnlock();
    return 0;
}

}
}