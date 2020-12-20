//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_EVENTREGISTER_H
#define PUMP_EVENT_EVENTREGISTER_H
#include "pump_event/pump_event_config.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_collector.h"
#include "pump_event/newly_event_mgr.h"
//#include "pump_event/event_engine.h"

namespace Pump
{
namespace Event
{

class CEventEngine;
class CEventCollector;
class CNewlyEvMgrGuiderForInner;

class PUMP_EVENT_CLASS CEventRegister
    : public ::Pump::Core::CNonCopyOperator
{
public:
    explicit CEventRegister(CEventEngine *pEvEngine);

    ~CEventRegister();

    //  /**
    //   * @fn insert
    //   * @param [in] hEv
    //   * @return 接口调用状态码
    //   * @brief 此接口要求线程安全,是用户向事件循环发出操作请求的入口
    //   */
    //  int insert_event(CEvent *__p_ev);

    /**
     * @fn register_events
     * @return 接口调用状态码
     * @brief 此接口供事件引擎调用,每没轮的事件循环执行周期中,注册Newly事件
     */
    int register_events();

    //private:
    //  int process_sync_event(CEvent *__p_ev);
    //
    //  int process_async_event(CEvent *__p_ev);

private:
    CEventEngine *const m_pEvEngine; ///< 绑定事件引擎(非所有者，禁止释放)，构造时绑定
    CEventCollector & m_refEvCollector; ///< 绑定事件收集器，构造时绑定
    CNewlyEvMgrGuiderForInner m_objNewEvMgrGuider; ///< 新注册事件容器Guider，构造时绑定容器
};

}
}

#endif // PUMP_EVENT_EVENTREGISTER_H
