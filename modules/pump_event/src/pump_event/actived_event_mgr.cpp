//
// Created by yz on 19-5-26.
//

#include "pump_event/actived_event_mgr.h"

namespace Pump
{
namespace Event
{

CActivedEventPriorQueue::CActivedEventContainer::CActivedEventContainer()
    : m_itWork(m_objActiveEventContainer.begin()) {}

CEvent *CActivedEventPriorQueue::CActivedEventContainer::begin()
{
    if (m_objActiveEventContainer.empty())
    {
        return PUMP_NULL;
    }
    return *m_objActiveEventContainer.begin();
}

CEvent *CActivedEventPriorQueue::CActivedEventContainer::end()
{
    if (m_objActiveEventContainer.empty())
    {
        return PUMP_NULL;
    }
    return *m_objActiveEventContainer.end();
}

bool CActivedEventPriorQueue::CActivedEventContainer::empty() const
{
    return m_objActiveEventContainer.empty();
}

size_t CActivedEventPriorQueue::CActivedEventContainer::size() const
{
    return m_objActiveEventContainer.size();
}

CEvent *CActivedEventPriorQueue::CActivedEventContainer::get()
{
    return *m_itWork;
}

CEvent *CActivedEventPriorQueue::CActivedEventContainer::next()
{
    if (m_itWork == m_objActiveEventContainer.end())
    {
        return *m_objActiveEventContainer.end();
    }
    m_itWork++;
    return *m_itWork;
}

CEvent *CActivedEventPriorQueue::CActivedEventContainer::prior()
{
    if (m_itWork == m_objActiveEventContainer.begin())
    {
        return *m_objActiveEventContainer.begin();
    }
    m_itWork--;
    return *m_itWork;
}

bool CActivedEventPriorQueue::CActivedEventContainer::clear()
{
    m_objActiveEventContainer.clear();
    return 0;
}

bool CActivedEventPriorQueue::CActivedEventContainer::insert(CEvent *pEv, const Condition *pCond)
{
    if (pEv == NULL)
    {
        return false;
    }
    m_objActiveEventContainer.push_back(pEv);
    return true;
}

bool CActivedEventPriorQueue::CActivedEventContainer::remove(CEvent *pEv, const Condition *pCond)
{
    if (pEv == NULL)
    {
        return false;
    }
    m_objActiveEventContainer.remove(pEv);
    return true;
}

CActivedEventPriorQueue::CActivedEventPriorQueue()
    : m_pWork(m_arrPriorQueue) {}

CActivedEventPriorQueue::CActivedEventContainer *CActivedEventPriorQueue::begin()
{
    return m_arrPriorQueue;
}

CActivedEventPriorQueue::CActivedEventContainer *CActivedEventPriorQueue::end()
{
    return m_arrPriorQueue + EVENT_PRIORQUEUE_LEN;
}

CActivedEventPriorQueue::CActivedEventContainer *CActivedEventPriorQueue::get()
{
    return m_pWork;
}

CActivedEventPriorQueue::CActivedEventContainer *CActivedEventPriorQueue::next()
{
    if (m_arrPriorQueue > m_arrPriorQueue + EVENT_PRIORQUEUE_LEN)
    {
        return m_arrPriorQueue + EVENT_PRIORQUEUE_LEN;
    }
    ++m_pWork;
    return m_pWork;
}

CActivedEventPriorQueue::CActivedEventContainer *CActivedEventPriorQueue::prior()
{
    if (m_pWork <= m_arrPriorQueue)
    {
        return m_arrPriorQueue;
    }
    --m_pWork;
    return m_pWork;
}

bool CActivedEventPriorQueue::insert(CEvent *pEv, const Condition *pCond)
{
    if (pEv == NULL)
    {
        return false;
    }
    m_arrPriorQueue[pEv->m_emEvPeriod].insert(pEv);
    return true;
}

bool CActivedEventPriorQueue::remove(CEvent *pEv, const Condition *pCond)
{
    if (pEv == NULL)
    {
        return false;
    }
    m_arrPriorQueue[pEv->m_emEvPeriod].remove(pEv);
    return true;
}

CActivedEventMgr::CActivedEventMgr() {}

CActivedEventMgr::~CActivedEventMgr() {}

bool CActivedEventMgr::insert(CEvent *__p_ev)
{
    m_csOffscreen.Lock();
    get_offscreen_unlocked().get()->insert(__p_ev);
    m_csOffscreen.Unlock();
    return true;
}

bool CActivedEventMgr::remove(CEvent *__p_ev)
{
    bool ret = get_work_locked().get()->remove(__p_ev);
    unlock_work();
    return ret;
}

void CActivedEventMgr::lock_offscreen()
{
    m_csOffscreen.Lock();
}

void CActivedEventMgr::unlock_offscreen()
{
    m_csOffscreen.Unlock();
}

CActivedEventMgr::buf_iterator CActivedEventMgr::get_offscreen_unlocked()
{
    if (m_pWork == &m_objActiveEventConatiner_0)
    {
        return CActivedEventMgr::buf_iterator(&m_objActiveEventConatiner_1);
    }
    else
    {
        return CActivedEventMgr::buf_iterator(&m_objActiveEventConatiner_0);
    }
}

void CActivedEventMgr::swap()
{
    MutexLocker(m_muxWork, m_csOffscreen);
    if (m_pWork == &m_objActiveEventConatiner_0)
    {
        setWorkUnlocked(&m_objActiveEventConatiner_1);
    }
    else
    {
        setWorkUnlocked(&m_objActiveEventConatiner_0);
    }
    MutexUnlocker(m_muxWork, m_csOffscreen);
}

}
}