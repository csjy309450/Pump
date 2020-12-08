//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_FDEVENT_H
#define PUMP_EVENT_FDEVENT_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_handle.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/ev_def.h"
#include "pump_event/event.h"
#include "pump_event/handle_event/handle_def.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{
const char *get_uuid();

typedef void(*pfn_event_callback)(CEvent *, void *, const size_t);

/**
 * @struct FdEvent []
 * @brief IO �¼�,��װfd��ص����Լ�����
 */
PUMP_ABSTRACT
class PUMP_EVENT_CLASS CHandleEvent
    : public CEvent
{
public:
    /**
     * @var pump_fd_t m_fd
     * @brief io�ļ�������
     */
    pump_fd_t m_fd;
    ::Pump::Core::CHandleBase * m_pHandle;
    /**
     * @var unsigned short re_fd_ev_
     * @brief io�ļ��������¼�fd�¼�����
     * - FDEV_TYPE_ACCEPT
     * - FDEV_TYPE_OPEN
     * - FDEV_TYPE_READ
     * - FDEV_TYPE_WRITE
     * - FDEV_TYPE_CLOSE
     * - FDEV_TYPE_SHUTDOWN
     */
    const FdEventType m_emFdEvType;

    CHandleEvent(
        ::Pump::Core::CHandleBase * pHandle,
        CEvent::ConditionType emCt,
        bool bAsync,
        FdEventType emEvType,
        EventPriority emEvPrior,
        CEvent::PeriodType emPt = ONETIME);

    /**
     *
     * @return FdEventType -> system io event
     * - PUMP_IOEV_IN
     * - PUMP_IOEV_OUT
     * - PUMP_IOEV_ERR
     */
    unsigned short get_io_event_type();
};

/**
* @brief [һ�����¼�] Socket�����¼�,�����ʼ��FileDescriptor����
*/
class PUMP_EVENT_CLASS CHandleInsert
    : public CHandleEvent
{
public:
    /**
    * @brief �½��׽��ֵĲ���, ͬ���¼�����,����ջ�϶���,Event�������ͷ��ڴ�
    */
    struct SockInsertArg
    {

    };
public:
    CHandleInsert();

    CHandleInsert(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior = EVPRIOR_DEFAULT);

    virtual ~CHandleInsert();

    virtual int _on_newed(CEventEngine &refEvEngine, EvData objData);
};

/**
* @brief [һ�����¼�] Fd Close�¼�,�������FileDescriptor
*/
class PUMP_EVENT_CLASS CHandleRemove
    : public CHandleEvent
{
public:
    CHandleRemove();

    CHandleRemove(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior = EVPRIOR_DEFAULT);

    virtual ~CHandleRemove();

    virtual int _on_newed(CEventEngine &refEvEngine, EvData objData);
};

}
}
}

#endif //PUMP_EVENT_FDEVENT_H
