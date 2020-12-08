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
        //TODO: err 参数错误
        goto ret_bad;
    }
    pLEContainer = refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(this);
    if (pLEContainer == NULL)
    {
        //TODO: err 未找到条件事件关联的事件监听容器
        goto ret_bad;
    }
    pHashContainer = (CHandleEventHashContainer *)pLEContainer->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err 未找到与 pump_event 匹配的 Listener
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
        // 如若 pFDescriptor 正处于open状态则表示已经初始化
        //TODO: err:重复初始化错误码
        goto ret_bad;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " before insert";
#endif //_TEST_LEVEL_DEBUG
    // 创建一个FileDescriptor对象,并插入到pHashContainer

    fDescriptor.m_pHandle = this->m_pHandle;
    fDescriptor.m_mapEv.clear();
    fDescriptor.m_emState = FD_STATE_TIMER_OPEN;
    pHashContainer->insert(&fDescriptor);
    fDescriptor.insert(this);

    pEvListener = dynamic_cast<CHandleEventListener *>(
        refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));// 获取fd事件监听器
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
        //TODO: err 定时器出错误
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

    // 2.获取事件监听器
    pEvListener = dynamic_cast<CHandleEventListener *>(refEvEngine.get_listener_by_uuid_locked(this->m_szUuid));
    if (pEvListener == NULL)
    {
        // TODO: err 未找到对应事件监听器
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        goto ret_bad;
    }

    // 3.从多路复用对象删除fd
    change.fd_ = this->m_fd;
    change.type_ = ::Pump::Core::Net::PUMP_FDCTL_DEL;
    if (pEvListener->get_backend()->update(change) == -1)
    {
        /* FIXME 设置错误码 */
        refEvEngine.unlock_listener_by_uuid(this->m_szUuid);
        PUMP_CORE_INFO << "fd 跟新到 MutiplexBackend 失败";
        goto ret_bad;
    }


#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "call ::close() " << this->m_fd;
#endif //_TEST_LEVEL_DEBUG

    // 1.关闭文件描述符
#ifdef __linux__
    // 调用close()接口, TODO: 处理系统调用返回
    switch (::close(this->m_fd))
    {
    case 0:
        break;
    default:
        break;
    }
#endif //__linux__

    // 4.获取文件描述符容器
    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(
        this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err:未找到与 pump_event 匹配的 Listener
        goto ret_bad;
    }

    // 5.移除timer文件描述符
    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_fd;
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor == NULL)
    {
        //TODO: err: 未找到需要删除的FileDescriptor对象
        goto ret_bad;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO << "pHashContainer size == " << pHashContainer->size() << " before remove";
#endif //_TEST_LEVEL_DEBUG
    // 重置 pFDescriptor 状态为未初始化
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