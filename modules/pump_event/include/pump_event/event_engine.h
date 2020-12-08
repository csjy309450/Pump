//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_EVENTENGINE_H
#define PUMP_EVENT_EVENTENGINE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_collector.h"
#include "pump_event/event_register.h"
#include "pump_event/event_listener_mgr.h"
#include "pump_event/event_reactor.h"

namespace Pump
{
namespace Event
{

//class CEventCollector;
//class CEventListener;
//class CEventListenerMgr;

/*
 * @brief 事件模型驱动引擎，封装三大核心组件，
 * 对外提供注册/注销事件、注册/注销监听器接口
 */
class PUMP_EVENT_CLASS CEventEngine
    : public ::Pump::Core::CNonCopyable
{
public:
    CEventEngine();

    ~CEventEngine();

    int routine();

    void bind_thread(pump_thread_id tid);

    pump_thread_id get_tid() const;

    int insert_event(CEvent *pEv);

    int remove_event(CEvent *pEv);

    int insert_listener(CEventListener *pListener);

    int remove_listener(CEventListener *pListener);

    CEventListener *get_listener_by_uuid_locked(const char *szUuid);

    void unlock_listener_by_uuid(const char *szUuid);

    CEventCollector &get_event_collector();

private:
    CEventCollector m_objEvCollector;
    CEventRegister m_objEvRegister;
    CEventListenerMgr m_objEvListenerMgr;
    CEventReactor m_objEvReactor;
    pump_thread_id m_tid;
};

}
}

#endif // PUMP_EVENT_EVENTENGINE_H
