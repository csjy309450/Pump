//
// Created by yz on 19-6-28.
//
#include "pump_macro/pump_pre.h"
#include <stdlib.h>
#include "pump_core/network/pump_core_sock.h"
#include "pump_event/event_interface.h"
#include "pump_event/handle_event/handle_cap.h"
#include "pump_event/handle_event/handle_event.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

CHandleCap::CHandleCap()
    : m_fdEv(0)
    //, m_fd((pump_fd_t)-1)
    , m_pHandle(PUMP_NULL)
    , m_emState(FD_STATE_DEFAULT) {}

CHandleCap::CHandleCap(const CHandleCap &other)
    : m_fdEv(other.m_fdEv)
    //, m_fd(other.m_fd)
    , m_pHandle(other.m_pHandle)
    , m_emState(other.m_emState)
    , m_mapEv(other.m_mapEv)
{

}

CHandleCap::~CHandleCap()
{
    clear();
}

CHandleCap &
CHandleCap::operator=(const CHandleCap &other)
{
    m_fdEv = other.m_fdEv;
    //m_fd = (other.m_fd);
    m_pHandle = (other.m_pHandle);
    m_emState = (other.m_emState);
    m_mapEv = (other.m_mapEv);
    return *this;
}

CEvent *CHandleCap::begin() { return NULL; }

CEvent *CHandleCap::end() { return NULL; }

const CEvent *CHandleCap::cbegin() const { return NULL; }

const CEvent *CHandleCap::cend() const { return NULL; }

bool CHandleCap::anchor(CEvent *) { return false; }

CEvent *CHandleCap::get() { return NULL; }

CEvent *CHandleCap::next() { return NULL; }

const CEvent *CHandleCap::cnext() { return NULL; }

CEvent *CHandleCap::prior() { return NULL; }

const CEvent *CHandleCap::cprior() { return NULL; }

CEvent *CHandleCap::find(const Condition *pCond)
{
    if (pCond == NULL ||
        (pCond != NULL && pCond->dwSize_ != sizeof(CHandleCap::FindByFdEventCond)))
    {
        //TODO: err 参数错误
        return NULL;
    }
    const CHandleCap::FindByFdEventCond *pFCond = static_cast<const CHandleCap::FindByFdEventCond *>(pCond);
    CHandleCap::ContainerType::iterator item = m_mapEv.find(pFCond->fdEvType_);
    if (item == m_mapEv.end())
    {
        //TODO: err 查找条件有误
        return NULL;
    }
    CEvent *pEv = m_mapEv.find(pFCond->fdEvType_)->second;
    return pEv;
}

bool CHandleCap::clear()
{
    // 逐个销毁事件
    for (CHandleCap::IteratorType it = m_mapEv.begin();
        it != m_mapEv.end(); it = m_mapEv.begin())
    {
        CEvent::EvData data = (*it).second->m_evData[EVSTATE_DESTROYED];
        pump_thread_id tid = (pump_thread_id)data.pData_;
        CEventEngine *pEngine = get_event_engine_by_tid_locked(tid);
        if (pEngine == NULL)
        {
            //TODO: err 未找到绑定 CEventEngine
            return false;
        }
        else
        {
            (*it).second->_on_destroyed(*pEngine, CEvent::EvData());
            unlock_event_engine_by_tid(tid);
        }
        m_mapEv.erase(it);
    }
    //m_fd = (pump_fd_t)-1;
    m_pHandle = PUMP_NULL;
    m_emState = FD_STATE_DEFAULT;
    /* 不能单纯清空,还需要销毁关联的事件 m_mapEv.clear();*/
    return true;
}

bool CHandleCap::insert(CEvent *pEv, const Condition *pCond)
{
    if (pEv == NULL)
    {
        // TODO: err 参数错误
        return false;
    }
    HandleEvent::CHandleEvent *pFdEv = dynamic_cast<HandleEvent::CHandleEvent *>(pEv);
    //if (pFdEv->m_fd != this->m_fd)
    if (pFdEv->m_pHandle != this->m_pHandle)
    {
        // TODO: err 参数错误
        return false;
    }
    if (this->m_mapEv.find(pFdEv->m_emFdEvType) != this->m_mapEv.end())
    {
        // TODO: err 找到重复元素
        return false;
    }
    this->m_mapEv.insert(HandleEvent::CHandleCap::ItemType(pFdEv->m_emFdEvType, pFdEv));
    return true;
}

bool CHandleCap::remove(CEvent *pEv, const Condition *pCond)
{
    if (pEv == NULL)
    {
        // TODO: err 参数错误
        return false;
    }
    HandleEvent::CHandleEvent *pFdEv = dynamic_cast<HandleEvent::CHandleEvent *>(pEv);
    if (pFdEv->m_pHandle != this->m_pHandle)
    {
        // TODO: err 参数错误
        return false;
    }
    if (this->m_mapEv.erase(pFdEv->m_emFdEvType) != 1)
    {
        // TODO: err 未找到元素
        return false;
    }
    return true;
}

}
}
}