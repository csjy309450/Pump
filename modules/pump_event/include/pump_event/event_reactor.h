//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_EVENTREACTOR_H
#define PUMP_EVENT_EVENTREACTOR_H
#include "pump_event/pump_event_config.h"
#include "pump_event/event_collector.h"
#include "pump_event/actived_event_mgr.h"

namespace Pump
{
namespace Event
{

class PUMP_EVENT_CLASS CEventReactor
{
public:
    CEventReactor(CEventEngine *pEvEngine);

    ~CEventReactor();

    //  int insert_event(CEvent *__p_ev);
    //
    //  int remove_event(CEvent *__p_ev);

    /**
    * @return 接口调用状态码
    * @brief 此接口供事件引擎调用,在每轮的事件循环执行周期中,处理Actived事件
    */
    int react_events();

private:
    CEventEngine *const m_pEvEngine; ///< 绑定事件引擎(非所有者，禁止释放)，构造时绑定
    CEventCollector &m_refEvCollector; ///< 绑定事件收集器，构造时绑定
    CActivedEventMgr &m_refActivedEvMgr; ///< 激活态事件管理器引用，构造时绑定
};

}
}

#endif // PUMP_EVENT_EVENTREACTOR_H
