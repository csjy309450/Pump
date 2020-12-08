//
// Created by yz on 19-7-22.
//
#include "pump_macro/pump_pre.h"
#ifdef __linux__
#include <unistd.h>
#elif (defined _WIN32)
#endif // __linux__
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_event/handle_event/timer_event.h"
#include "pump_event/handle_event/handle_cap.h"
#include "pump_event/event_engine.h"
#include "pump_event/listened_event_mgr.h"
#include "pump_event/handle_event/handle_event_listener.h"
#include "pump_event/handle_event/handle_event_container.h"
#include "pump_core/network/pump_core_multiplex.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{
CTimerCreate::CTimerCreate()
    : CHandleEvent(PUMP_NULL, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_TIMER_CREART, EVPRIOR_DEFAULT, ONETIME)
{

}

CTimerCreate::CTimerCreate(::Pump::Core::CHandleBase * pHandle, CEvent::PeriodType emPeriod)
    : CHandleEvent(pHandle, CONDITIONAL, PUMP_EVENT_ASYNC, FDEV_TYPE_TIMER_CREART, EVPRIOR_DEFAULT, emPeriod)
{

}

CTimerCreate::~CTimerCreate()
{

}

int CTimerCreate::_on_newed(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "timer_create::_on_newed() in";
#endif //_TEST_LEVEL_DEBUG
    CHandleCap fDescriptor;
    CListenedEventContainer *pLEContainer = NULL;
    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;
    CHandleEventListener *pEvListener = NULL;
    ::Pump::Core::Net::CFdCtl change;

    if (objData.pData_ != NULL && objData.dwSize_ != sizeof(TimerCreateArg))
    {
        //TODO: err ��������
        goto ret_bad;
    }
    pLEContainer = refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(this);
    if (pLEContainer == NULL)
    {
        //TODO: err δ�ҵ������¼��������¼���������
        goto ret_bad;
    }
    pHashContainer = (CHandleEventHashContainer *)pLEContainer->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err δ�ҵ��� pump_event ƥ��� Listener
        goto ret_bad;
    }

    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_fd;
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor == NULL || ((pFDescriptor->m_pHandle->GetHandle() >= 0 && (size_t)pFDescriptor->m_pHandle->GetHandle() < g_nFd)
        && (pFDescriptor->m_emState != FD_STATE_CLOSED
        && pFDescriptor->m_emState != FD_STATE_DEFAULT)))
    {
        // ���� pFDescriptor ������open״̬���ʾ�Ѿ���ʼ��
        //TODO: err:�ظ���ʼ��������
        goto ret_bad;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " before insert";
#endif //_TEST_LEVEL_DEBUG
    // ����һ��FileDescriptor����,�����뵽pHashContainer

    fDescriptor.m_pHandle = this->m_pHandle;
    fDescriptor.m_mapEv.clear();
    fDescriptor.m_emState = FD_STATE_TIMER_OPEN;
    pHashContainer->insert(&fDescriptor);
    fDescriptor.insert(this);

    pEvListener = dynamic_cast<CHandleEventListener *>(
        refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));// ��ȡfd�¼�������
    pEvListener->get_backend()->update(change);
    change.fd_ = this->m_fd;
    change.type_ = ::Pump::Core::Net::PUMP_FDCTL_ADD;
    change.fd_ev_ = ::Pump::Core::Net::PUMP_IOEV_IN;
    pEvListener->get_backend()->update(change);
    refEvEngine.unlock_listener_by_uuid(this->m_szUuid);

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " after insert";
    PUMP_CORE_INFO << "timer_create::_on_newed() out with " << 1;
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "timer_create::_on_newed() out with " << -1;
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

int CTimerCreate::_on_actived(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "timer_create::_on_actived() in";
#endif //_TEST_LEVEL_DEBUG
#ifdef __linux__
    uint64_t exp;
    ssize_t ret = ::read(this->m_fd, &exp, sizeof(uint64_t));
    if (ret != 8)
    {
        //TODO: err ��ʱ��������
    }
#elif __WIN32

#endif // __linux__

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "timer_create::_on_actived() out with " << 1;
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "timer_create::_on_actived() out with " << -1;
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

int CTimerCreate::_on_destroyed(CEventEngine &refEvEngine, EvData objData)
{

#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "fd_close::_on_actived() in";
#endif //_TEST_LEVEL_DEBUG
    CHandleEventListener *pEvListener = NULL;
    ::Pump::Core::Net::CFdCtl change;

    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;

    // 2.��ȡ�¼�������
    pEvListener = dynamic_cast<CHandleEventListener *>(refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));
    if (pEvListener == NULL)
    {
        // TODO: err δ�ҵ���Ӧ�¼�������
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        goto ret_bad;
    }

    // 3.�Ӷ�·���ö���ɾ��fd
    change.fd_ = this->m_fd;
    change.type_ = ::Pump::Core::Net::PUMP_FDCTL_DEL;
    if (pEvListener->get_backend()->update(change) == -1)
    {
        /* FIXME ���ô����� */
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        PUMP_CORE_INFO << "fd ���µ� MutiplexBackend ʧ��";
        goto ret_bad;
    }


#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "call ::close() " << this->m_fd;
#endif //_TEST_LEVEL_DEBUG

    // 1.�ر��ļ�������
#ifdef __linux__
    // ����close()�ӿ�, TODO: ����ϵͳ���÷���
    switch (::close(this->m_fd))
    {
    case 0:
        break;
    default:
        break;
    }
#endif //__linux__

    // 4.��ȡ�ļ�����������
    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(
        this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err:δ�ҵ��� pump_event ƥ��� Listener
        goto ret_bad;
    }

    // 5.�Ƴ�timer�ļ�������
    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_fd;
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor == NULL)
    {
        //TODO: err: δ�ҵ���Ҫɾ����FileDescriptor����
        goto ret_bad;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " before remove";
#endif //_TEST_LEVEL_DEBUG
    // ���� pFDescriptor ״̬Ϊδ��ʼ��
    pHashContainer->remove(pFDescriptor);

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " after remove";
    PUMP_CORE_INFO << "fd_close::_on_actived() out with " << 1;
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "fd_close::_on_actived() out with " << -1;
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

}
}
}