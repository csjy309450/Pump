//
// Created by yz on 19-5-26.
//

#include "pump_event/listened_event_mgr.h"
#include "pump_event/event_container.h"
#include <algorithm>

namespace Pump
{
namespace Event
{

CListenedEventContainer::CListenedEventContainer() {}

CListenedEventContainer::~CListenedEventContainer() {}

CListenedEventContainer::CListenedEventContainer(const CListenedEventContainer & other) {}

CListenedEventContainer &
CListenedEventContainer::operator=(const CListenedEventContainer & other) { return *this; }

CListenedEventMgr::CListenedEventMgr() {}

CListenedEventMgr::~CListenedEventMgr() {}

CListenedEventMgr::CListenedEventMgr(const CListenedEventMgr & other) {}

CListenedEventMgr &CListenedEventMgr::operator=(const CListenedEventMgr &other) { return *this; }

bool CListenedEventMgr::insert_event_container(
    CEventContainer::EventContainerType emContainerType,
    pump_handle_t hRealContainer,
    const char *szUuid)
{
    if (hRealContainer == 0)
    {
        return false;
    }
    CListenedEventContainer *pEvConatiner = new CListenedEventContainer();
    pEvConatiner->m_emContainerType = emContainerType;
    pEvConatiner->m_pEvConatiner = (void *)hRealContainer;
    pEvConatiner->m_szUuid = szUuid;
    m_mapListenedEv.insert(ItemType((pump_handle_t)hRealContainer, pEvConatiner));
    return true;
}

bool CListenedEventMgr::remove_event_container(pump_handle_t hEvConatiner)
{
    CEventContainer *it = m_mapListenedEv.find(hEvConatiner)->second;
    if (it == NULL)
    {
        // TODO: ²ÎÊı´íÎó
        return false;
    }
    if (m_mapListenedEv.erase(hEvConatiner) <= 0)
    {
        // TODO: É¾³ıÊ§°Ü
        return false;
    }
    delete it;
    return true;
}

CListenedEventContainer *CListenedEventMgr::find_event_container(const CEvent *pEv)
{
    if (pEv == NULL)
    {
        return NULL;
    }
    CListenedEventMgr::IteratorType it = m_mapListenedEv.begin();
    for (; it != m_mapListenedEv.end(); ++it)
    {
        if (strcmp(it->second->m_szUuid, pEv->m_szUuid) == 0)
        {
            return it->second;
        }
    }
    //Î´ÕÒµ½¶ÔÓ¦¼àÌıÈİÆ÷
    return NULL;
}

bool CListenedEventMgr::insert_event(CEvent *pEv)
{
    if (pEv == NULL)
    {
        return false;
    }

    bool ret = false;
    m_csListenedEv.Lock();
    for (MapType::iterator it = m_mapListenedEv.begin();
        it != m_mapListenedEv.end(); ++it)
    {
        //    (*it).second->m_csThis.lock();
        if (strcmp(pEv->m_szUuid, it->second->m_szUuid) == 0)
        {
            switch (it->second->m_emContainerType)
            {
            case CEventContainer::Multidim:
            {
                ret = ((CMultidimEventContainer *)it->second->m_pEvConatiner)->insert(pEv);
            }
            break;
            case CEventContainer::Unidim:
            {
                ret = ((CUnidimEventContainer *)it->second->m_pEvConatiner)->insert(pEv);
            }
            break;
            default:
                break;
            }
        }
        //    (*it).second->m_csThis.unlock();
    }
    m_csListenedEv.Unlock();
    return ret;
}

bool CListenedEventMgr::remove_event(CEvent *__p_ev)
{
    return false;
}

}
}