//
// Created by yz on 19-6-22.
//
#include "pump_macro/pump_pre.h"
#include <stdlib.h>
#include <ctype.h>
#include <map>
#include "pump_event/event_engine.h"
#include "pump_event/event_interface.h"

namespace Pump
{
namespace Event
{

CEventEngine *get_event_engine_by_tid_locked(pump_thread_id tid)
{
  CEventCollector *p_ev_collector = get_event_collector_by_tid_locked(tid);
  if (p_ev_collector == NULL)
  {
    return NULL;
  }
  return &p_ev_collector->get_event_engine();
}

int unlock_event_engine_by_tid(pump_thread_id tid)
{
  return unlock_event_collector_by_tid(tid);
}

}
}