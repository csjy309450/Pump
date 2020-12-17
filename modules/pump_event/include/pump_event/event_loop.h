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
* CEventLoop�¼�ѭ������������һ���̣߳��̵߳���Ҫ�����������¼����棬
* ִ���¼�ѭ����
*/
class PUMP_EVENT_CLASS CEventLoop
    : public Pump::Core::Thread::CThread
{
private:
    enum LoopState
    {
        LOOPSTATE_STOP = 0, ///< ��ֹ
        LOOPSTATE_START, ///< ����
        LOOPSTATE_PAUSE ///< ��ͣ
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
    /**�¼�ѭ��ִ��״̬*/
    volatile pump_uint32_t m_dwState;
    /**�¼�ѭ��������*/
    ::Pump::Core::Thread::CMutex m_csThis;
    /**�¼�����*/
    CEventEngine m_objEvEngine;
};

}
}

#endif //PUMP_EVENT_EVENTLOOP_H
