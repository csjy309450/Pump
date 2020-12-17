//
// Created by yz on 19-5-26.
//
#include <stdlib.h>
#include "pump_core/pump_core_logger.h"
#include "pump_event/event_collector.h"
#include "pump_event/event_engine.h"
#include "pump_core/pump_core_multiplex.h"
#include "pump_event/handle_event/socket_event.h"
#include "pump_event/handle_event/handle_event_container.h"
#include "pump_event/handle_event/handle_event_listener.h"
#include "pump_event/event_interface.h"
#include "pump_event/event_loop.h"
#include "pump_event/handle_event/handle_event.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

static const char *g_szUUID = "b8c5ed57-8466-4780-9b88-158bc06191fd";

const char *get_uuid()
{
    return g_szUUID;
}

CHandleEvent::CHandleEvent(::Pump::Core::CHandleBase *pHandle,
    CEvent::ConditionType emCt,
    bool bAsync, FdEventType emEvType,
    EventPriority emEvPrior,
    CEvent::PeriodType emPt)
    : m_pHandle(pHandle)
    , CEvent(emCt, bAsync, get_uuid()
    , emEvPrior, emPt)
    , m_emFdEvType(emEvType)
{

}

unsigned short CHandleEvent::get_io_event_type()
{
    switch (this->m_emFdEvType)
    {
    case HandleEvent::FdEventType::FDEV_TYPE_SOCK_ACCEPT:
    case HandleEvent::FdEventType::FDEV_TYPE_SOCK_CONNECT:
    case HandleEvent::FdEventType::FDEV_TYPE_SOCK_RECV:
    case HandleEvent::FdEventType::FDEV_TYPE_FILE_READ:
        return ::Pump::Core::Net::PUMP_IOEV_IN;
    case HandleEvent::FdEventType::FDEV_TYPE_FILE_WRITE:
        return ::Pump::Core::Net::PUMP_IOEV_OUT;
    default:
        return ::Pump::Core::Net::PUMP_IOEV_NONE;
    }
}


CHandleInsert::CHandleInsert()
    : CHandleEvent(PUMP_NULL, NONCONDITIONAL, PUMP_EVENT_SYNC, FDEV_TYPE_SOCK_CREART, EVPRIOR_DEFAULT, ONETIME)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("new sock_create -> %ul" , this);
#endif //_TEST_LEVEL_DEBUG
}

CHandleInsert::CHandleInsert(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior)
    : CHandleEvent(pHandle, NONCONDITIONAL, PUMP_EVENT_SYNC, FDEV_TYPE_SOCK_CREART, emPrior, ONETIME)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("new sock_create -> %ul" , this);
#endif //_TEST_LEVEL_DEBUG
}

CHandleInsert::~CHandleInsert()
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("delete sock_create -> %ul" , this);
#endif //_TEST_LEVEL_DEBUG
}

int CHandleInsert::_on_newed(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CHandleInsert::_on_newed() in");
#endif //_TEST_LEVEL_DEBUG
    CHandleCap fDescriptor;
    CListenedEventContainer *pLEContainer = NULL;
    CMultidimEventContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;

    if (objData.pData_ != NULL && objData.dwSize_ != sizeof(SockInsertArg))
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
    pHashContainer = (CMultidimEventContainer *)pLEContainer->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err 未找到与 pump_event 匹配的 Listener
        goto ret_bad;
    }

    fCond.dwSize_ = sizeof(fCond);
    fCond.fd_ = this->m_fd;
    pFDescriptor = dynamic_cast<CHandleCap *>(
        pHashContainer->find(&fCond));
    if (pFDescriptor != NULL && ((pFDescriptor->m_pHandle->Available())
        && (pFDescriptor->m_emState != FD_STATE_CLOSED
        && pFDescriptor->m_emState != FD_STATE_DEFAULT)))
    {
        // 如若 pFDescriptor 正处于open状态则表示已经初始化
        //TODO: err:重复初始化错误码
        goto ret_bad;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("pHashContainer size == %d before insert", pHashContainer->size());
#endif //_TEST_LEVEL_DEBUG
    // 创建一个FileDescriptor对象,并插入到pHashContainer
    fDescriptor.m_pHandle = m_pHandle;
    //fDescriptor.m_fd = this->m_fd;
    fDescriptor.m_mapEv.clear();
    fDescriptor.m_emState = FD_STATE_SOCK_INITED;
    pHashContainer->insert(&fDescriptor);

    // TODO 需要调用句柄

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("pHashContainer size == %d after insert" , pHashContainer->size());
    PUMP_CORE_INFO("sock_create::_on_actived() out with 1");
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("sock_create::_on_actived() out with -1");
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

CHandleRemove::CHandleRemove()
    : CHandleEvent(PUMP_NULL, NONCONDITIONAL, PUMP_EVENT_SYNC, FDEV_TYPE_CLOSE, EVPRIOR_DEFAULT, ONETIME)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("new fd_close -> %ul" ,this);
#endif //_TEST_LEVEL_DEBUG
}

CHandleRemove::CHandleRemove(::Pump::Core::CHandleBase * pHandle, EventPriority emPrior)
    : CHandleEvent(pHandle, NONCONDITIONAL, PUMP_EVENT_SYNC, FDEV_TYPE_CLOSE, emPrior, ONETIME)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("new fd_close -> %ul" , this);
#endif //_TEST_LEVEL_DEBUG
}

CHandleRemove::~CHandleRemove()
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("delete fd_close -> %ul" ,this);
#endif //_TEST_LEVEL_DEBUG
}

int CHandleRemove::_on_newed(CEventEngine &refEvEngine, EvData objData)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("fd_close::_on_newed() in");
#endif //_TEST_LEVEL_DEBUG
    CHandleEventHashContainer *pHashContainer = NULL;
    ::Pump::Event::HandleEvent::FindCond fCond;
    CHandleCap *pFDescriptor = NULL;

    pHashContainer = (CHandleEventHashContainer *)refEvEngine.get_event_collector().get_listened_event_mgr().find_event_container(
        this)->m_pEvConatiner;
    if (pHashContainer == NULL)
    {
        // TODO: err:未找到与 pump_event 匹配的 Listener
        goto ret_bad;
    }

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
    PUMP_CORE_INFO("pHashContainer size == %d before remove" , pHashContainer->size());
#endif //_TEST_LEVEL_DEBUG
    // 重置 pFDescriptor 状态为未初始化
    pHashContainer->remove(pFDescriptor);

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("pHashContainer size == %d after remove" , pHashContainer->size());
    PUMP_CORE_INFO("fd_close::_on_newed() out with 1");
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("fd_close::_on_newed() out with -1");
#endif //_TEST_LEVEL_DEBUG
    return -1;
}


}
}
}