//
// Created by yz on 19-7-22.
//

#ifndef PUMP_EVENT_TIMER_EVENT_H
#define PUMP_EVENT_TIMER_EVENT_H
#include "pump_event/pump_event_config.h"
#include "pump_event/handle_event/handle_event.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

class PUMP_EVENT_CLASS CTimerCreate
    : public CHandleEvent
{
public:
    /**
     * @brief �½��׽��ֵĲ���, ͬ���¼�����,����ջ�϶���,Event�������ͷ��ڴ�
     */
    struct TimerCreateArg
    {

    };
public:
    CTimerCreate();

    CTimerCreate(::Pump::Core::CHandleBase * pHandle, CEvent::PeriodType emPeriod);

    virtual ~CTimerCreate();

private:
    virtual int _on_newed(CEventEngine &refEvEngine, EvData objData);

    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData);

    virtual int _on_destroyed(CEventEngine &refEvEngine, EvData objData);
};

}
}
}

#endif // PUMP_EVENT_TIMER_EVENT_H
