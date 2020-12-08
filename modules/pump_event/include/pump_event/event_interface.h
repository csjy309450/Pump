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
 * 向EventHub注册事件收集器，上母写锁->放母写锁
 */
int register_event_collector(CEventCollector *pEvCollector);

/*
* 从EventHub删除事件收集器，上母写锁->上子锁->放母写锁，然后去除（不允许释放，非事件收集器所有者）
* @param tid 事件收集器所属CEventEngine绑定的TID
*/
int unregister_event_collector_by_tid(pump_thread_id tid);

/**
* 获取事件搜集器，上母读锁->上子锁
* @param tid 事件收集器所属CEventEngine绑定的TID
*/
CEventCollector *get_event_collector_by_tid_locked(pump_thread_id tid);

/**
* 获取事件搜集器，上母读锁->放子锁->放母读锁->放母读锁
* @param tid 事件收集器所属CEventEngine绑定的TID
*/
int unlock_event_collector_by_tid(pump_thread_id tid);

}
}

#endif //PUMP_EVENT_INTERFACE_H
