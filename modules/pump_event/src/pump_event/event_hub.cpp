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
        CSharedMutex m_muxSon; //! m_mapEvEngines������,��Ҫ��ĳ��StruItemԪ�ؽ����޸�ʱ������,ɾ��ʱҪ����ĸ��ͬ�¼���
        __CHubItem() : m_muxSon() {}

        //FIXME: ���� __CHubItem ���Ա�����,����m_muxSon���ظ��ͷ�
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
    ::Pump::Core::Thread::CRWLocker m_muxMom; //! m_mapEvCollector��ĸ��, �����½ڵ���ĸ��, ������Ҫ������ĸ��,Ȼ���ͷ�ĸ��

private:
    static __CEventHub * s_evHub; ///< EventHubȫ�ֵ���
    static ::Pump::Core::Thread::CMutex s_csEvHub; ///< EventHubȫ�ֹ�����
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
        //TODO: err �ڴ����ʧ��
    }
    s_csEvHub.Unlock();
    return s_evHub;
}

int register_event_collector(CEventCollector *pEvCollector)
{
    if (pEvCollector == NULL)
    {
        // TODO: err ��������
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
        // TODO: err δ�ҵ���Ӧ�̺߳Ű󶨵�event_engine
        __CEventHub::__get_global_event_hub()->m_muxMom.writeUnlock();
        return -1;
    }
    res->second.m_muxSon.Lock();
    // TODO [����]: �˴���ȷ��,�����ͷ��ܲ�������???
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
        // TODO: err δ�ҵ���Ӧ�̺߳Ű󶨵�event_engine
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
        // TODO: err δ�ҵ���Ӧ�̺߳Ű󶨵�event_engine
        __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
        return -1;
    }
    res->second.m_muxSon.Unlock();
    // �������Σ���Ϊ��ȡ��ʱ���Ѿ�����һ�ζ���
    __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
    __CEventHub::__get_global_event_hub()->m_muxMom.readUnlock();
    return 0;
}

}
}