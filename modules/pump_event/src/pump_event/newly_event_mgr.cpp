//
// Created by yz on 19-5-26.
//
#include "pump_memory/smart_buffer/circular_buffer_interface.hpp"
#include "pump_event/event.h"
#include "pump_event/newly_event_mgr.h"

//using namespace std;
//using namespace Pump::Memory;
//using namespace Pump::Core::Thread;

namespace Pump
{
namespace Event
{

CNewlyEventMgr::CNewlyEventMgr()
{
    this->setWorkLocked(&m_listNewlyEv_0);
}

CNewlyEventMgr::~CNewlyEventMgr() {}

bool CNewlyEventMgr::insert(CEvent *pEv)
{
    m_csOffscreen.Lock();
    get_offscreen_unlocked().get()->push_back(pEv);
    m_csOffscreen.Unlock();
    return true;
}

CNewlyEventMgr::buf_iterator CNewlyEventMgr::get_offscreen_unlocked()
{
    if (m_pWork == &m_listNewlyEv_0)
    {
        return CNewlyEventMgr::buf_iterator(&m_listNewlyEv_1);
    }
    else
    {
        return CNewlyEventMgr::buf_iterator(&m_listNewlyEv_0);
    }
}

void CNewlyEventMgr::swap()
{
    MutexLocker(m_muxWork, m_csOffscreen);
    if (m_pWork == &m_listNewlyEv_0)
    {
        setWorkUnlocked(&m_listNewlyEv_1);
    }
    else
    {
        setWorkUnlocked(&m_listNewlyEv_0);
    }
    MutexUnlocker(m_muxWork, m_csOffscreen);
}

CNewlyEvMgrGuiderForInner::CNewlyEvMgrGuiderForInner(CNewlyEventMgr &refNEMgr)
    : m_refNewlyEvMgr(refNEMgr) {}

bool CNewlyEvMgrGuiderForInner::insert(CEvent *pEv)
{
    return m_refNewlyEvMgr.insert(pEv);
}

void CNewlyEvMgrGuiderForInner::swap()
{
    m_refNewlyEvMgr.swap();
}

CNewlyEventMgr::BufferIteratorType CNewlyEvMgrGuiderForInner::get_work_locked()
{
    return m_refNewlyEvMgr.get_work_locked();
}

void CNewlyEvMgrGuiderForInner::unlock_work()
{
    m_refNewlyEvMgr.unlock_work();
}

CNewlyEvMgrGuiderForUser::CNewlyEvMgrGuiderForUser(CNewlyEventMgr &refNEMgr)
    : m_refNewlyEvMgr(refNEMgr) {}

bool CNewlyEvMgrGuiderForUser::insert(CEvent *pEv)
{
    return m_refNewlyEvMgr.insert(pEv);
}

#ifdef _TEST_LEVEL_DEBUG

CNewlyEventMgr::BufferIteratorType CNewlyEvMgrGuiderForUser::get_offscreen_locked()
{
    m_refNewlyEvMgr.m_csOffscreen.Lock();
    return m_refNewlyEvMgr.get_offscreen_unlocked();
}

void CNewlyEvMgrGuiderForUser::unlock_offscreen()
{
    m_refNewlyEvMgr.m_csOffscreen.Unlock();
}

#endif //_TEST_LEVEL_DEBUG

}
}