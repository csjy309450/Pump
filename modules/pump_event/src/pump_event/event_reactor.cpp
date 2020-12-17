//
// Created by yz on 19-5-26.
//
#include "pump_event/event_reactor.h"
#include <stdlib.h>
#include "pump_event/event_interface.h"
#include "pump_event/event_engine.h"
#include "pump_core/pump_core_logger.h"

namespace Pump
{
namespace Event
{


CEventReactor::CEventReactor(CEventEngine *pEvEngine)
    : m_pEvEngine(pEvEngine),
    m_refEvCollector(pEvEngine->get_event_collector()),
    m_refActivedEvMgr(m_refEvCollector.get_actived_event_mgr()) {}

CEventReactor::~CEventReactor() {}

//int CEventReactor::insert_event(CEvent *__p_ev)
//{
//  if (__p_ev == NULL)
//  {
//    //TODO: ��Ч���
//    return -1;
//  }
//  bool ret = m_refActivedEvMgr.insert(__p_ev);
//  return (ret ? 1 : -1);
//}
//
//int CEventReactor::remove_event(CEvent *__p_ev)
//{
//  if (__p_ev == NULL)
//  {
//    //TODO: ��Ч���
//    return -1;
//  }
//  bool ret = m_refActivedEvMgr.remove(__p_ev);
//  if (ret)
//  {
//    CEvent::EvData data = __p_ev->m_evData[EVSTATE_DESTROYED];
//    pump_thread_id tid = (pump_thread_id) data.pData;
//    CEventEngine *pEngine = get_event_engine_by_tid_locked(tid);
//    if (pEngine == NULL)
//    {
//      //TODO: err δ�ҵ��� CEventEngine
//      return -1;
//    } else
//    {
//      __p_ev->_on_destroyed(*pEngine, __p_ev->m_evData[EVSTATE_DESTROYED]);
//      unlock_event_engine_by_tid(tid);
//    }
//  }
//  return (ret ? 1 : -1);
//}

int CEventReactor::react_events()
{
    // 1.����ǰ��̨����
    m_refActivedEvMgr.swap();
    // 2.����˫����ǰ̨����
    CActivedEventMgr::buf_iterator container = m_refActivedEvMgr.get_work_locked().get();
    // 3.�����������ȼ�����
    for (CActivedEventPriorQueue::CActivedEventContainer *it = container.get()->begin();
        it != container.get()->end(); it++)
    {
        // 3.1 ���������¼�����
        for (CEvent *pEv = (*it).begin(); pEv != (*it).end(); pEv = (*it).next())
        {
            if (pEv->m_emEvState != EVSTATE_ACTIVATED || pEv->m_emEvCondition != CEvent::NONCONDITIONAL)
            {
                //TODO: �¼�����,�˴�ֻӦ���ּ���̬���������¼�
                assert(1 == 0);
                continue;
            }
            // 3.1.1 ���ü���ص�����
            pEv->_on_actived(*m_pEvEngine, CEvent::EvData());
            // 3.1.2 ���������ɵ�һ�����¼�
            switch (pEv->m_emEvPeriod)
            {
            case CEvent::ONETIME:
            {
                m_refActivedEvMgr.remove(pEv);
                pEv->m_emEvState = EVSTATE_SOLVED;
                pEv->_on_solved(*m_pEvEngine, CEvent::EvData());
            }
            break;
            case CEvent::CONTINUOUS:
            {
                pEv->m_emEvState = EVSTATE_SOLVED;
                pEv->_on_solved(*m_pEvEngine, CEvent::EvData());
                pEv->m_emEvState = EVSTATE_ACTIVATED;
            }
            break;
            default:
                break;
            }
        }
        // 3.2 �����ʱ�����¼������Բ�Ϊ�գ�����¼����뵽��̨����������һ��ѭ���ٴ���
        if (!(*it).empty())
        {
            m_refActivedEvMgr.lock_offscreen();
            CActivedEventMgr::buf_iterator itOffscreen = m_refActivedEvMgr.get_offscreen_unlocked();
            for (CEvent *pEv = (*it).begin(); pEv != (*it).end(); pEv = (*it).next())
            {
                itOffscreen.get()->insert(pEv);
            }
            m_refActivedEvMgr.unlock_offscreen();
            (*it).clear();
        }
    }
    m_refActivedEvMgr.unlock_work();
    return 0;
}

}
}