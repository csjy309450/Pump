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
 * @brief �¼�����Ļ��࣬�¼�����������ջ�϶�����
 * DONE : [task-001] �¼�ȱ���û��ص�, ʹ��EventHandler����, ���¼�:���==1:N
 * �û��ص�ʹ��CFunction����֧�ֲ�ͬCEvent�󶨲�ͬ�����Ļص�������ˣ��û�
 * �ص���CEvent���������ж���.
 */
class PUMP_EVENT_CLASS CEvent
    : public Pump::Core::CNonCopyable
{
public:
    /**
    * @brief Event��������
    */
    struct EvData
    {
        size_t dwSize_;   //! PData�ĳ���
        bool bDel_;       //! ������OnXXX()�����Ƿ��ͷŲ���pData
        void *pData_;     //! �����ڴ�<��������>,�������ͷ�
    };

    enum PeriodType
    {
        ONETIME,    //! һ�����¼�
        CONTINUOUS, //! �������¼�
    };

    enum ConditionType
    {
        NONCONDITIONAL, //! �������¼�
        CONDITIONAL,    //! �����¼�
    };

    const enum EventPriority m_emEvPriority; //! < �¼������ȼ�

    const bool m_bAsync; //!< ͬ�����첽

    enum EventState m_emEvState; //!< �¼���״̬

    enum PeriodType m_emEvPeriod; //!< �¼�������������<������>

    const enum ConditionType m_emEvCondition; //!< �¼�������������<���ɸ���>

    const char *m_szUuid; //!< �¼�����Դ��, ��Listennerƥ��

    CEvent(ConditionType emCt, bool bAsync, const char *szUuid, EventPriority emPrior,
        PeriodType emPt = ONETIME);

    CEvent();

    virtual ~CEvent();

    // <deprecate-001> OnXXXϵ�нӿ�Ϊ�ں�ͬ������,���Բ����е�EvData���ں˴���,���û�����
    void set_data(EventState emEvState, EvData objData);

    EvData &get_data(EventState emEvState);

    /**
    * @var EvData evData_
    * @brief ָ���û�����
    *
    * ��Event�¼���������Ĺ�����ָ��,<��������>
    */
    EvData m_evData[6];
    //</deprecate-001>

    /**
    * @param [in] eventCollector �¼��ռ���
    * @param [in] objData ������ص������Ĳ���,�� evData_[?] ��
    * @brief �¼�����NewlyMgr��Ļص�,��Event�¼���������Ĺ�����ָ��
    *
    */
    virtual int _on_newed(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] eventCollector �¼��ռ���
    * @param [in] objData ������ص������Ĳ���,�� evData_[?] ��
    * @brief �¼�����Listener��Ļص�,��Event�¼���������Ĺ�����ָ��
    *
    */
    virtual int _on_listend(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] refEvEngine �¼��ռ���
    * @param [in] objData ������ص������Ĳ���,�� evData_[?] ��
    * @brief �¼�����Reactor��Ļص�,��Event�¼���������Ĺ�����ָ��
    *
    */
    virtual int _on_actived(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] refEvEngine �¼��ռ���
    * @param [in] objData ������ص������Ĳ���,�� evData_[?] ��
    * @brief �¼��������Ļص�,��Event�¼���������Ĺ�����ָ��
    *
    */
    virtual int _on_suspended(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] eventCollector �¼��ռ���
    * @param [in] objData ������ص������Ĳ���,�� evData_[?] ��
    * @brief �¼�������ɺ�Ļص�,��Event�¼���������Ĺ�����ָ��
    *
    */
    virtual int _on_solved(CEventEngine &refEvEngine, EvData objData) { return 0; }

    /**
    * @param [in] eventCollector �¼��ռ���
    * @param [in] objData ������ص������Ĳ���, evData_[EVSTATE_DESTROYED]
    * @brief �¼����ٺ�Ļص�,��Event�¼���������Ĺ�����ָ��
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