//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_EVENTLISTENERMGR_H
#define PUMP_EVENT_EVENTLISTENERMGR_H
#include <vector>
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_rwlocker.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_collector.h"
#include "pump_event/event_listener.h"
#include "pump_event/utils/shared_mutex.h"

//using namespace std;
//using namespace Pump::ThreadMgr;

namespace Pump
{
namespace Event
{

const unsigned short g_maxEvListner = 10;

class PUMP_EVENT_CLASS CEventListenerMgr
    : public Pump::Core::CNonCopyOperator
{
public:
    class CListenerItem
    {
    public:
        CEventListener *m_pEvListener;
        CSharedMutex m_csItem;

        CListenerItem() : m_csItem() {}

        ~CListenerItem() {}
    };

    typedef std::vector<CListenerItem> ContainerType;
    typedef std::vector<CListenerItem>::iterator IteratorType;
private:
    class CFindPredicate
    {
    public:
        explicit CFindPredicate(CEventListener *pListener)
            : m_pListener(pListener) {}

        bool operator()(const ContainerType::value_type &item)
        {
            return (item.m_pEvListener == m_pListener);
        }

    private:
        CEventListener *m_pListener;
    };

public:
    explicit CEventListenerMgr(CEventEngine *pEvEngine);

    ~CEventListenerMgr();

    /**插入新监听器，上写锁*/
    int insert_event_listener(CEventListener *pEvListener);

    /**移除监听器，上写锁*/
    int remove_event_listener(CEventListener *pEvListener);

    /**获取监听器，上读锁，不允许向监听器容器增删*/
    CEventListener *get_listener_by_uuid_locked(const char *szUuid);

    /**解锁监听器*/
    void unlock_listener_by_uuid(const char *szUuid);

    //  int insert_event(CEvent *__p_ev);
    //
    //  int remove_event(CEvent *__p_ev);

    /**执行监听业务*/
    int listen_event();

private:
    CEventEngine *const m_pEvEngine; ///< 绑定事件引擎(非所有者，禁止释放)，构造时绑定
    CEventCollector &m_refEvCollector; ///< 绑定事件收集器(非所有者，禁止释放)，构造时绑定
    ::Pump::Core::Thread::CRWLocker m_csEvListenerContainer;///< 监听器容器的读写锁，因为经常有按照uuid获取监听器的操作，所以采用读写锁提高并发能力
    ContainerType m_objEvListenerContainer;///< 监听器容器
};

}
}

#endif //PUMP_EVENT_EVENTLISTENERMGR_H
