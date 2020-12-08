//
// Created by yz on 19-5-26.
//
#include <stdlib.h>
#include "pump_event/event_register.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_event/event_engine.h"
#include "pump_event/event_collector.h"

class CEventCollector;

namespace Pump
{
namespace Event
{

CEventRegister::CEventRegister(CEventEngine *pEvEngine)
    : m_pEvEngine(pEvEngine),
    m_refEvCollector(pEvEngine->get_event_collector()),
    m_objNewEvMgrGuider(m_refEvCollector.get_newly_event_mgr())
{
}

CEventRegister::~CEventRegister()
{
}

//int CEventRegister::insert_event(CEvent *__p_ev)
//{
//  if (__p_ev == NULL)
//  {
//    // TODO : err 参数错误
//    return -1;
//  }
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::insert_event() in with pump_event == " << __p_ev;
//#endif //_TEST_LEVEL_DEBUG
//  int ret = -1;
//  if (__p_ev->m_bAsync)
//  {
//    ret = process_async_event(__p_ev);
//  } else
//  {
//    ret = process_sync_event(__p_ev);
//  }
//
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::insert_event() out";
//#endif //_TEST_LEVEL_DEBUG
//  return ret;
//}

int CEventRegister::register_events()
{
    // 1.交换双缓冲中主从缓冲区
    m_objNewEvMgrGuider.swap();

    // 2.锁住工作缓冲区, FIXED: 之前此处没使用引用, 勿使用了拷贝复制, 导致 Newly pump_event Container 注册后未被清空
    CNewlyEventMgr::ContainerType &container = *m_objNewEvMgrGuider.get_work_locked().get();

    // 3.遍历Newly事件容器, 根据事件类型加入到对应容器中
    for (CNewlyEventMgr::ContainerIteratorType it = container.begin();
        it != container.end(); it++)
    {
        if (!(*it)->m_bAsync)
        {
            // 此函数中只处理异步事件,打印错误
            PUMP_CORE_ERR << "同步事件 " << (pump_ev_handle)*it << " 插入错误";
            // 删除非法事件
            container.erase(it);
            continue;
        }

        // 3.1 根据事件触发条件, 加入到对应事件管理器
        switch ((*it)->m_emEvCondition)
        {
        case CEvent::NONCONDITIONAL:
        {
            //无条件事件，插入事件到激活事件管理器
            if (!m_refEvCollector.get_actived_event_mgr().insert(*it))
            {
                // TODO: err 事件插入错误
            }
            else
            {
                (*it)->m_emEvState = EVSTATE_ACTIVATED;
            }
        }
        break;
        case CEvent::CONDITIONAL:
        {
            //条件事件，插入到监听事件管理器
            if (!m_refEvCollector.get_listened_event_mgr().insert_event(*it))
            {
                // TODO: err 事件插入错误
                PUMP_CORE_ERR << "条件事件 " << (pump_ev_handle)*it << " 插入错误";
            }
            else
            { // 注册条件事件成功
                (*it)->m_emEvState = EVSTATE_LISTENED;
                CEvent::EvData data = { 0 };
                (*it)->_on_listend(*m_pEvEngine, data);
            }
        }
        break;
        default:
        {
            // TODO: err 事件条件类型错误
        }
        }
        // 3.2 从Newly事件管理器中移除该事件,更新迭代器
        it = container.erase(it);
        if (it == container.end())
        {
            break;
        }
    }

#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_LOG_ASSERT(container.size() == 0) << "Newly pump_event work container size expect:0 actual:" << container.size();
#endif //_TEST_LEVEL_DEBUG

    m_objNewEvMgrGuider.unlock_work();
    return 0;
}

//int CEventRegister::process_sync_event(CEvent *__p_ev)
//{
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::process_sync_event() in";
//#endif //_TEST_LEVEL_DEBUG
//
//  CEvent::EvData data = {0};
//
//  if (__p_ev == NULL)
//  {
//    //TODO: 参数错误
//    goto ret_bad;
//  }
//
//ret_good:
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::process_sync_event() out with " << 1;
//#endif //_TEST_LEVEL_DEBUG
//  return __p_ev->_on_actived(*m_pEvEngine, data);
//
//ret_bad:
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::process_sync_event() out with " << -1;
//#endif //_TEST_LEVEL_DEBUG
//  return -1;
//}
//
//int CEventRegister::process_async_event(CEvent *__p_ev)
//{
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::process_async_event() in";
//#endif //_TEST_LEVEL_DEBUG
//  if (__p_ev == NULL)
//  {
//    //TODO: 参数错误
//    goto ret_bad;
//  }
//  if (!m_objNewEvMgrGuider.insert(__p_ev))
//  {
//    //TODO: 插入新事件失败
//    goto ret_bad;
//  }
//
//ret_good:
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::process_async_event() out with " << 1;
//#endif //_TEST_LEVEL_DEBUG
//  return 0;
//
//ret_bad:
//#ifdef _TEST_LEVEL_DEBUG
//  PUMP_CORE_INFO << "CEventRegister::process_async_event() out with " << -1;
//#endif //_TEST_LEVEL_DEBUG
//  return -1;
//}

}
}