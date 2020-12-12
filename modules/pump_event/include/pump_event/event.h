//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_EVENT_H
#define PUMP_EVENT_EVENT_H
#include "pump_macro/pump_pre.h"
#include "pump_event/pump_event_config.h"
#include <string>
#include "pump_core/pump_core_noncopyable.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_handler.h"

namespace Pump
{
namespace Event
{

class CEventEngine;

#define PUMP_EVENT_ASYNC true
#define PUMP_EVENT_SYNC false

/**
 * @brief 事件对象的基类，事件对象不允许以栈上对象构造
 * DONE : [task-001] 事件缺少用户回调, 使用EventHandler对象, 且事件:句柄==1:N
 * 用户回调使用CFunction对象，支持不同CEvent绑定不同参数的回调对象。因此，用户
 * 回调在CEvent的派生类中定义.
 */
class PUMP_EVENT_CLASS CEvent
    : public Pump::Core::CNonCopyable
{
public:
    /**
    * @brief Event附加数据
    */
    struct EvData
    {
        size_t dwSize_;   //! PData的长度
        bool bDel_;       //! 调用完OnXXX()函数是否释放参数pData
        void *pData_;     //! 参数内存<非所有者>,不允许释放
    };

    enum PeriodType
    {
        ONETIME,    //! 一次性事件
        CONTINUOUS, //! 持续性事件
    };

    enum ConditionType
    {
        NONCONDITIONAL, //! 无条件事件
        CONDITIONAL,    //! 条件事件
    };

    const enum EventPriority m_emEvPriority; //! < 事件的优先级

    const bool m_bAsync; //!< 同步或异步

    enum EventState m_emEvState; //!< 事件的状态

    enum PeriodType m_emEvPeriod; //!< 事件触发周期属性<可设置>

    const enum ConditionType m_emEvCondition; //!< 事件触发条件属性<不可更改>

    const char *m_szUuid; //!< 事件的资源号, 与Listenner匹配

    CEvent(ConditionType emCt, bool bAsync, const char *szUuid, EventPriority emPrior,
        PeriodType emPt = ONETIME);

    CEvent();

    virtual ~CEvent();

    // <deprecate-001> OnXXX系列接口为内核同步调用,所以参数中的EvData由内核传递,非用户传递
    void set_data(EventState emEvState, EvData objData);

    EvData &get_data(EventState emEvState);

    /**
    * @var EvData evData_
    * @brief 指向用户数据
    *
    * 由Event事件派生对象的构造者指定,<非所有者>
    */
    EvData m_evData[6];
    //</deprecate-001>

    /**
    * @param [in] eventCollector 事件收集器
    * @param [in] objData 输入给回调函数的参数,非 evData_[?] 中
    * @brief 事件加入NewlyMgr后的回调,由Event事件派生对象的构造者指定
    *
    */
    virtual int _on_newed(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] eventCollector 事件收集器
    * @param [in] objData 输入给回调函数的参数,非 evData_[?] 中
    * @brief 事件加入Listener后的回调,由Event事件派生对象的构造者指定
    *
    */
    virtual int _on_listend(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] refEvEngine 事件收集器
    * @param [in] objData 输入给回调函数的参数,非 evData_[?] 中
    * @brief 事件加入Reactor后的回调,由Event事件派生对象的构造者指定
    *
    */
    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] refEvEngine 事件收集器
    * @param [in] objData 输入给回调函数的参数,非 evData_[?] 中
    * @brief 事件被挂起后的回调,由Event事件派生对象的构造者指定
    *
    */
    virtual int _on_suspended(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] eventCollector 事件收集器
    * @param [in] objData 输入给回调函数的参数,非 evData_[?] 中
    * @brief 事件处理完成后的回调,由Event事件派生对象的构造者指定
    *
    */
    virtual int _on_solved(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] eventCollector 事件收集器
    * @param [in] objData 输入给回调函数的参数, evData_[EVSTATE_DESTROYED]
    * @brief 事件销毁后的回调,由Event事件派生对象的构造者指定
    *
    */
    virtual int _on_destroyed(CEventEngine &refEvEngine, EvData objData) { return 0; }

    //void bind(CEventHandler *pHandler) { m_pHandler = pHandler; }
    //void unbind() { m_pHandler = NULL; }
    //bool empty() const { return (m_pHandler == NULL); }
};

}
}

#endif //PUMP_EVENT_EVENT_H