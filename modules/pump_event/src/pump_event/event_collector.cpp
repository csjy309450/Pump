//
// Created by yz on 19-5-26.
//

#include "pump_event/event_collector.h"
#include "pump_core/logger/pump_core_logger.h"

namespace Pump
{
namespace Event
{

CEventCollector::CEventCollector(CEventEngine &refEvEngine)
    : m_refEvEngine(refEvEngine) {}

CEventEngine &CEventCollector::get_event_engine()
{
    return m_refEvEngine;
}

CNewlyEventMgr &CEventCollector::get_newly_event_mgr()
{
    return m_objNewlyEvMgr;
}

CListenedEventMgr &CEventCollector::get_listened_event_mgr()
{
    return m_objListenedEvMgr;
}

CActivedEventMgr &CEventCollector::get_actived_event_mgr()
{
    return m_objActivedEvMgr;
}

int CEventCollector::insert_newly_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        // TODO : err 参数错误
        return -1;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::insert_event() in with pump_ev == %ul" , pEv);
#endif //_TEST_LEVEL_DEBUG
    int ret = -1;
    if (pEv->m_bAsync)
    {
        ret = __process_async_newly_event(pEv);
    }
    else
    {
        ret = __process_sync_newly_event(pEv);
    }

#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::insert_event() out");
#endif //_TEST_LEVEL_DEBUG
    return ret;
}

int CEventCollector::__process_sync_newly_event(CEvent *pEv)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::process_sync_event() in");
#endif //_TEST_LEVEL_DEBUG

    if (pEv == NULL)
    {
        //TODO: 参数错误
        goto ret_bad;
    }

//ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::process_sync_event() out with 1");
#endif //_TEST_LEVEL_DEBUG
    return pEv->_on_newed(this->get_event_engine(), CEvent::EvData());

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::process_sync_event() out with -1");
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

int CEventCollector::__process_async_newly_event(CEvent *pEv)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::process_async_event() in");
#endif //_TEST_LEVEL_DEBUG
    CNewlyEvMgrGuiderForInner ne_mgr_guider(this->m_objNewlyEvMgr);
    if (pEv == NULL)
    {
        //TODO: 参数错误
        goto ret_bad;
    }
    if (!ne_mgr_guider.insert(pEv))
    {
        //TODO: 插入新事件失败
        goto ret_bad;
    }
    pEv->_on_newed(m_refEvEngine, CEvent::EvData());

//ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::process_async_event() out with 1");
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CEventRegister::process_async_event() out with -1");
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

int CEventCollector::insert_actived_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        //TODO: 无效句柄
        return -1;
    }
    bool ret = m_objActivedEvMgr.insert(pEv);
    return (ret ? 1 : -1);
}

int CEventCollector::remove_actived_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        //TODO: 无效句柄
        return -1;
    }
    bool ret = m_objActivedEvMgr.remove(pEv);
    if (ret)
    {
        pEv->_on_destroyed(
            this->get_event_engine(),
            CEvent::EvData());
    }
    return (ret ? 1 : -1);
}

int CEventCollector::insert_listened_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        //TODO: err 参数错误
        return -1;
    }
    m_objListenedEvMgr.insert_event(pEv);
    return 0;
}

int CEventCollector::remove_listened_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        //TODO: err 参数错误
        return -1;
    }
    bool ret = m_objListenedEvMgr.remove_event(pEv);
    if (ret)
    {
        pEv->_on_destroyed(
            this->get_event_engine(),
            CEvent::EvData());
    }
    return (ret ? 1 : -1);
}

}
}