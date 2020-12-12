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
 * @brief �¼��ռ������¼�����ģ�͵ĺ���������
 *   ���� ���������������� �¼�������
 *   �¼��Ѽ������¼������������ע�ᵽEventHub�У���ȫ�ִ��ڶ��
 *   EventLoopʱ����EventHub�����ص�����
 * @note CEventCollector�Ĳ����������̰߳�ȫ��
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
     * @return �ӿڵ���״̬��
     * @brief �˽ӿ�Ҫ���̰߳�ȫ,���û����¼�ѭ������������������
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
    CEventEngine & m_refEvEngine; ///< Ψһ��Ӧ�¼����棬����ʱ��
    CNewlyEventMgr m_objNewlyEvMgr;
    CListenedEventMgr m_objListenedEvMgr;
    CActivedEventMgr m_objActivedEvMgr;
};

}
}

#endif //PUMP_EVENT_EVENTCOLLECTOR_H
