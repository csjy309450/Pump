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
    * @return �ӿڵ���״̬��
    * @brief �˽ӿڹ��¼��������,��ÿ�ֵ��¼�ѭ��ִ��������,����Actived�¼�
    */
    int react_events();

private:
    CEventEngine *const m_pEvEngine; ///< ���¼�����(�������ߣ���ֹ�ͷ�)������ʱ��
    CEventCollector &m_refEvCollector; ///< ���¼��ռ���������ʱ��
    CActivedEventMgr &m_refActivedEvMgr; ///< ����̬�¼����������ã�����ʱ��
};

}
}

#endif // PUMP_EVENT_EVENTREACTOR_H
