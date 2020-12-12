//
// Created by yz on 19-5-26.
//

#include "pump_event/event.h"
#include "pump_event/event_interface.h"

namespace Pump
{
namespace Event
{

CEvent::CEvent(ConditionType emCt, bool bAsync, const char *szUuid, EventPriority emPrior, PeriodType emPt)
  : m_emEvCondition(emCt),
    m_bAsync(bAsync),
    m_szUuid(szUuid),
    m_emEvPriority(emPrior),
    m_emEvPeriod(emPt) {}

CEvent::~CEvent()
{
}

CEvent::CEvent()
  : m_emEvCondition(NONCONDITIONAL),
    m_bAsync(false),
    m_szUuid(""),
    m_emEvPriority(EVPRIOR_DEFAULT),
    m_emEvPeriod(ONETIME) {}

void CEvent::set_data(EventState emEvState, EvData objData)
{
  m_evData[emEvState] = objData;
}

CEvent::EvData &CEvent::get_data(EventState emEvState)
{
  return m_evData[emEvState];
}

}
}