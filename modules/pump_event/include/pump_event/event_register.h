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
    //   * @return �ӿڵ���״̬��
    //   * @brief �˽ӿ�Ҫ���̰߳�ȫ,���û����¼�ѭ������������������
    //   */
    //  int insert_event(CEvent *__p_ev);

    /**
     * @fn register_events
     * @return �ӿڵ���״̬��
     * @brief �˽ӿڹ��¼��������,ÿû�ֵ��¼�ѭ��ִ��������,ע��Newly�¼�
     */
    int register_events();

    //private:
    //  int process_sync_event(CEvent *__p_ev);
    //
    //  int process_async_event(CEvent *__p_ev);

private:
    CEventEngine *const m_pEvEngine; ///< ���¼�����(�������ߣ���ֹ�ͷ�)������ʱ��
    CEventCollector & m_refEvCollector; ///< ���¼��ռ���������ʱ��
    CNewlyEvMgrGuiderForInner m_objNewEvMgrGuider; ///< ��ע���¼�����Guider������ʱ������
};

}
}

#endif // PUMP_EVENT_EVENTREGISTER_H
