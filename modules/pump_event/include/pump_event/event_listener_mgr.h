//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_EVENTLISTENERMGR_H
#define PUMP_EVENT_EVENTLISTENERMGR_H
#include <vector>
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_rwlocker.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/ev_def.h"
#include "pump_event/event_collector.h"
#include "pump_event/event_listener.h"
#include "pump_event/utils/shared_mutex.h"

//using namespace std;
//using namespace Pump::ThreadMgr;

namespace Pump
{
namespace Event
{

const unsigned short g_maxEvListner = 10;

class PUMP_EVENT_CLASS CEventListenerMgr
    : public Pump::Core::CNonCopyOperator
{
public:
    class CListenerItem
    {
    public:
        CEventListener *m_pEvListener;
        CSharedMutex m_csItem;

        CListenerItem() : m_csItem() {}

        ~CListenerItem() {}
    };

    typedef std::vector<CListenerItem> ContainerType;
    typedef std::vector<CListenerItem>::iterator IteratorType;
private:
    class CFindPredicate
    {
    public:
        explicit CFindPredicate(CEventListener *pListener)
            : m_pListener(pListener) {}

        bool operator()(const ContainerType::value_type &item)
        {
            return (item.m_pEvListener == m_pListener);
        }

    private:
        CEventListener *m_pListener;
    };

public:
    explicit CEventListenerMgr(CEventEngine *pEvEngine);

    ~CEventListenerMgr();

    /**�����¼���������д��*/
    int insert_event_listener(CEventListener *pEvListener);

    /**�Ƴ�����������д��*/
    int remove_event_listener(CEventListener *pEvListener);

    /**��ȡ���������϶������������������������ɾ*/
    CEventListener *get_listener_by_uuid_locked(const char *szUuid);

    /**����������*/
    void unlock_listener_by_uuid(const char *szUuid);

    //  int insert_event(CEvent *__p_ev);
    //
    //  int remove_event(CEvent *__p_ev);

    /**ִ�м���ҵ��*/
    int listen_event();

private:
    CEventEngine *const m_pEvEngine; ///< ���¼�����(�������ߣ���ֹ�ͷ�)������ʱ��
    CEventCollector &m_refEvCollector; ///< ���¼��ռ���(�������ߣ���ֹ�ͷ�)������ʱ��
    ::Pump::Core::Thread::CRWLocker m_csEvListenerContainer;///< �����������Ķ�д������Ϊ�����а���uuid��ȡ�������Ĳ��������Բ��ö�д����߲�������
    ContainerType m_objEvListenerContainer;///< ����������
};

}
}

#endif //PUMP_EVENT_EVENTLISTENERMGR_H
