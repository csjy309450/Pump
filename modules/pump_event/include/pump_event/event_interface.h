//
// Created by yz on 19-6-22.
//

#ifndef PUMP_EVENT_INTERFACE_H
#define PUMP_EVENT_INTERFACE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_event/ev_def.h"

namespace Pump
{
namespace Event
{

class CEventEngine;

class CEventCollector;

CEventEngine *get_event_engine_by_tid_locked(pump_thread_id tid);

int unlock_event_engine_by_tid(pump_thread_id tid);

/*
 * ��EventHubע���¼��ռ�������ĸд��->��ĸд��
 */
int register_event_collector(CEventCollector *pEvCollector);

/*
* ��EventHubɾ���¼��ռ�������ĸд��->������->��ĸд����Ȼ��ȥ�����������ͷţ����¼��ռ��������ߣ�
* @param tid �¼��ռ�������CEventEngine�󶨵�TID
*/
int unregister_event_collector_by_tid(pump_thread_id tid);

/**
* ��ȡ�¼��Ѽ�������ĸ����->������
* @param tid �¼��ռ�������CEventEngine�󶨵�TID
*/
CEventCollector *get_event_collector_by_tid_locked(pump_thread_id tid);

/**
* ��ȡ�¼��Ѽ�������ĸ����->������->��ĸ����->��ĸ����
* @param tid �¼��ռ�������CEventEngine�󶨵�TID
*/
int unlock_event_collector_by_tid(pump_thread_id tid);

}
}

#endif //PUMP_EVENT_INTERFACE_H
