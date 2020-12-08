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
//    // TODO : err ��������
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
    // 1.����˫���������ӻ�����
    m_objNewEvMgrGuider.swap();

    // 2.��ס����������, FIXED: ֮ǰ�˴�ûʹ������, ��ʹ���˿�������, ���� Newly pump_event Container ע���δ�����
    CNewlyEventMgr::ContainerType &container = *m_objNewEvMgrGuider.get_work_locked().get();

    // 3.����Newly�¼�����, �����¼����ͼ��뵽��Ӧ������
    for (CNewlyEventMgr::ContainerIteratorType it = container.begin();
        it != container.end(); it++)
    {
        if (!(*it)->m_bAsync)
        {
            // �˺�����ֻ�����첽�¼�,��ӡ����
            PUMP_CORE_ERR << "ͬ���¼� " << (pump_ev_handle)*it << " �������";
            // ɾ���Ƿ��¼�
            container.erase(it);
            continue;
        }

        // 3.1 �����¼���������, ���뵽��Ӧ�¼�������
        switch ((*it)->m_emEvCondition)
        {
        case CEvent::NONCONDITIONAL:
        {
            //�������¼��������¼��������¼�������
            if (!m_refEvCollector.get_actived_event_mgr().insert(*it))
            {
                // TODO: err �¼��������
            }
            else
            {
                (*it)->m_emEvState = EVSTATE_ACTIVATED;
            }
        }
        break;
        case CEvent::CONDITIONAL:
        {
            //�����¼������뵽�����¼�������
            if (!m_refEvCollector.get_listened_event_mgr().insert_event(*it))
            {
                // TODO: err �¼��������
                PUMP_CORE_ERR << "�����¼� " << (pump_ev_handle)*it << " �������";
            }
            else
            { // ע�������¼��ɹ�
                (*it)->m_emEvState = EVSTATE_LISTENED;
                CEvent::EvData data = { 0 };
                (*it)->_on_listend(*m_pEvEngine, data);
            }
        }
        break;
        default:
        {
            // TODO: err �¼��������ʹ���
        }
        }
        // 3.2 ��Newly�¼����������Ƴ����¼�,���µ�����
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
//    //TODO: ��������
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
//    //TODO: ��������
//    goto ret_bad;
//  }
//  if (!m_objNewEvMgrGuider.insert(__p_ev))
//  {
//    //TODO: �������¼�ʧ��
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