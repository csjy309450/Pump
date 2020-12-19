//
// Created by yz on 19-7-21.
//

#include "pump_core/pump_core_mutex.h"
#include "pump_core/pump_core_rwlocker.h"
#include "pump_event/event_interface.h"
#include "pump_event/event_engine.h"
#include "pump_event/utils/shared_mutex.h"

namespace Pump
{
namespace Event
{

class __CEventHub
    : public Pump::Core::CNonCopyOperator
{
public:
    class __CHubItem
    {
    public:
        CEventCollector *m_pEvCollector;
        CSharedMutex m_muxSon; //! m_mapEvEngines的子锁,需要对某个StruItem元素进行修改时加子锁,删除时要求字母锁同事加锁
        __CHubItem() : m_muxSon() {}

        //FIXME: 由于 __CHubItem 可以被拷贝,导致m_muxSon配重复释放
        ~__CHubItem() {}
    };
#define ITEMTYPE pump_thread_id, __CHubItem
    typedef std::pair<ITEMTYPE> ItemType;
    typedef std::map<ITEMTYPE> MapType;
#undef ITEMTYPE
    typedef MapType::iterator IteratorType;

public:
    static __CEventHub * __get_global_event_hub();
public:
    __CEventHub() {}

    MapType m_mapEvCollector;
    ::Pump::Core::Thread::CRWLocker m_muxMom; //! m_mapEvCollector的母锁, 插入新节点上母锁, 查找需要先上字母锁,然后释放母锁

private:
    static __CEventHub * s_evHub; ///< EventHub全局单例
    static ::Pump::Core::Thread::CMutex s_csEvHub; ///< EventHub全局构造锁
};

__CEventHub * __CEventHub::s_evHub = NULL;
::Pump::Core::Thread::CMutex __CEventHub::s_csEvHub;

__CEventHub * __CEventHub::__get_global_event_hub()
{
    if (s_evHub != NULL)
    {
        return s_evHub;
    }
    s_csEvHub.Lock();
    if (s_evHub != NULL)
    {
        s_csEvHub.Unlock();
        return s_evHub;
    }
    s_evHub = new __CEventHub();
    if (s_evHub == NULL)
    {
        //TODO: err 内存分配失败
    }
    s_csEvHub.Unlock();
    return s_evHub;
}

int register_event_collector(CEventCollector *pEvCollector)
{
    if (pEvCollector == NULL)
    {
        // TODO: err 参数错误
        return -1;
    }

    __CEventHub::__CHubItem item;
    item.m_pEvCollector = pEvCollector;
    __CEventHub::__get_global_event_hub()->m_muxMom.writeLock();
    __CEventHub::__get_global_event_hub()->m_mapEvCollector.insert(
        __CEventHub::ItemType(pEvCollector->get_event_engine().get_tid(),
        item));
    __CEventHub::__get_global_event_hub()->m_muxMom.writeUnlock();
    return 0;
}

int unregister_event_collector_by_tid(pump_thread_id tid)
{
    if (__CEventHub::__get_global_event_hub()->m_mapEvCollector.empty())
    {
        return -1;
    }
    __CEventHub::__get_global_event_hub()->m_muxMom.writeLock();
    __CEventHub::IteratorType res = __CEventHub::__get_global_event_hub()->m_mapEvCollector.find(tid);
    if (res != __CEventHub::__get_global_event_hub()->m_mapEvCollector.end())
    {
        // TODO: err 未找到对应线程号绑定的event_engine
        __CEventHub::__get_global_event_hub()->m_muxMom.writeUnlock();
        return -1;
    }
    res->second.m_muxSon.Lock();
    // TODO [紧急]: 此处不确定,锁不释放能不能析构???
    __CEventHub::__get_global_event_hub()->m_mapEvCollector.erase(res);
    __CEventHub::__get_global_event_hub()->m_muxMom.writeUnlock();
    return 0;
}

CEventCollector *get_event_collector_by_tid_locked(pump_thread_id tid)
{
    __CEventHub::__get_global_event_hub()->m_muxMom.readLock();
    if (__CEventHub::__get_global_event_hub()->m_mapEvCollector.empty())
    {
        __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
        return NULL;
    }

    __CEventHub::IteratorType res = __CEventHub::__get_global_event_hub()->m_mapEvCollector.find(tid);
    if (res == __CEventHub::__get_global_event_hub()->m_mapEvCollector.end())
    {
        // TODO: err 未找到对应线程号绑定的event_engine
        __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
        return NULL;
    }
    res->second.m_muxSon.Lock();
    //  __CEventHub::__get_global_event_hub()->m_muxMom.writeUnlock();
    return res->second.m_pEvCollector;
}

int unlock_event_collector_by_tid(pump_thread_id tid)
{
    if (__CEventHub::__get_global_event_hub()->m_mapEvCollector.empty())
    {
        return -1;
    }
    __CEventHub::__get_global_event_hub()->m_muxMom.readLock();
    __CEventHub::IteratorType res = __CEventHub::__get_global_event_hub()->m_mapEvCollector.find(tid);
    if (res != __CEventHub::__get_global_event_hub()->m_mapEvCollector.end())
    {
        // TODO: err 未找到对应线程号绑定的event_engine
        __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
        return -1;
    }
    res->second.m_muxSon.Unlock();
    // 解锁两次，因为获取的时候已经上了一次读锁
    __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
    __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
    return 0;
}

}
}