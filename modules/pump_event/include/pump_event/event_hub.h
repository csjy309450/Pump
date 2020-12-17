//
// Created by yz on 19-7-21.
//

#ifndef PUMP_EVENT_HUB_H
#define PUMP_EVENT_HUB_H
#include <map>
#include "pump_core/pump_core_thread.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/event_collector.h"
#include "pump_event/ev_def.h"

namespace Pump
{
namespace Event
{

int register_event_collector(CEventCollector *pEvCollector);

int unregister_event_collector_by_tid(pump_thread_id tid);

CEventCollector *get_event_collector_by_tid_locked(pump_thread_id tid);

int unlock_event_engine_by_tid(pump_thread_id tid);

}
}

#endif //PUMP_EVENT_HUB_H
