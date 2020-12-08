//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_EVENTCOLLECTOR_H
#define PUMP_EVENT_EVENTCOLLECTOR_H
#include "pump_macro/pump_pre.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/listened_event_mgr.h"
#include "pump_event/newly_event_mgr.h"
#include "pump_event/actived_event_mgr.h"

namespace Pump
{
namespace Event
{

class CEventEngine;
class CNewlyEventMgr;
class CListenedEventMgr;
class CActivedEventMgr;

/*
 * @brief 事件收集器，事件驱动模型的核心容器。
 *   包含 新增、监听、激活 事件管理器
 *   事件搜集器在事件引擎启动后会注册到EventHub中，当全局存在多个
 *   EventLoop时，由EventHub处理负载的问题
 * @note CEventCollector的操作必须是线程安全的
 */
class PUMP_EVENT_CLASS CEventCollector
{
public:
    CEventCollector(CEventEngine &refEvEngine);

    CEventEngine &get_event_engine();

    CNewlyEventMgr &get_newly_event_mgr();

    CListenedEventMgr &get_listened_event_mgr();

    CActivedEventMgr &get_actived_event_mgr();

    /**
     * @fn insert
     * @param [in] hEv
     * @return 接口调用状态码
     * @brief 此接口要求线程安全,是用户向事件循环发出操作请求的入口
     */
    int insert_newly_event(CEvent *pEv);

    int insert_actived_event(CEvent *pEv);

    int remove_actived_event(CEvent *pEv);

    int insert_listened_event(CEvent *pEv);

    int remove_listened_event(CEvent *pEv);

private:
    int __process_sync_newly_event(CEvent *pEv);

    int __process_async_newly_event(CEvent *pEv);

private:
    CEventEngine & m_refEvEngine; ///< 唯一对应事件引擎，构造时绑定
    CNewlyEventMgr m_objNewlyEvMgr;
    CListenedEventMgr m_objListenedEvMgr;
    CActivedEventMgr m_objActivedEvMgr;
};

}
}

#endif //PUMP_EVENT_EVENTCOLLECTOR_H
