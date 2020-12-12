//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_LISTENEDEVENTMGR_H
#define PUMP_EVENT_LISTENEDEVENTMGR_H
#include <map>
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/event_container.h"

namespace Pump
{
namespace Event
{

class CEventContainer;

/**
* @brief 监听态事件容器
*/
class PUMP_EVENT_CLASS CListenedEventContainer
    : public CEventContainer
{
public:
    CListenedEventContainer();

    virtual ~CListenedEventContainer();

    const char *m_szUuid;

    ::Pump::Core::Thread::CMutex m_csThis;

private:
    CListenedEventContainer(const CListenedEventContainer & other);

    CListenedEventContainer &operator=(const CListenedEventContainer &other);
};

/**
* @brief 监听态事件容器管理器
*/
class PUMP_EVENT_CLASS CListenedEventMgr
{
public:
#define ITEMTYPE pump_handle_t, CListenedEventContainer*
    typedef std::map<ITEMTYPE> MapType;
    typedef std::map<ITEMTYPE>::iterator IteratorType;
    typedef std::pair<ITEMTYPE> ItemType;
#undef ITEMTYPE
public:

    CListenedEventMgr();

    ~CListenedEventMgr();

    /**
    * @param emContainerType
    * @param hRealContainer [in]
    * @return
    */
    bool insert_event_container(
        CEventContainer::EventContainerType emContainerType,
        pump_handle_t hRealContainer,
        const char *szUuid);

    bool remove_event_container(pump_handle_t hEvConatiner);

    CListenedEventContainer *find_event_container(const CEvent *);

    bool insert_event(CEvent *pEv);

    bool remove_event(CEvent *pEv);

#ifdef _TEST_LEVEL_DEBUG

    MapType &get_map_listened_event() { return m_mapListenedEv; }

#endif //_TEST_LEVEL_DEBUG

private:
    CListenedEventMgr(const CListenedEventMgr &other);

    CListenedEventMgr &operator=(const CListenedEventMgr &other);

private:
    ::Pump::Core::Thread::CMutex m_csListenedEv;
    MapType m_mapListenedEv; //! EventListener关联的EventContainer的所有者
};

}
}

#endif //PUMP_EVENT_LISTENEDEVENTMGR_H
