//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_EVENTLOOP_H
#define PUMP_EVENT_EVENTLOOP_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_thread.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_engine.h"

namespace Pump
{
namespace Event
{
class CEventEngine;

/**
* CEventLoop事件循环对象本质上是一个线程，线程的主要功能是驱动事件引擎，
* 执行事件循环。
*/
class PUMP_EVENT_CLASS CEventLoop
    : public Pump::Core::Thread::CThread
{
private:
    enum LoopState
    {
        LOOPSTATE_STOP = 0, ///< 终止
        LOOPSTATE_START, ///< 运行
        LOOPSTATE_PAUSE ///< 暂停
    };
public:
    CEventLoop();

    virtual ~CEventLoop();

    virtual pump_int32_t Start();

    virtual pump_int32_t Stop();

    pump_int32_t Pause();

    pump_int32_t Resume();

    pump_bool_t IsStop();

    pump_int32_t InsertEvent(pump_ev_handle hEv);

    pump_int32_t RemoveEvent(pump_ev_handle hEv);

    pump_int32_t InsertListener(pump_listener_handle hListener);

    pump_int32_t RemoveListener(pump_listener_handle hListener);

    CEventCollector &GetEventCollector();

private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData);
    pump_void_t SetState(LoopState emState);

private:
    /**事件循环执行状态*/
    volatile pump_uint32_t m_dwState;
    /**事件循环对象锁*/
    ::Pump::Core::Thread::CMutex m_csThis;
    /**事件引擎*/
    CEventEngine m_objEvEngine;
};

}
}

#endif //PUMP_EVENT_EVENTLOOP_H
