//
// Created by yz on 19-5-26.
//
#include <cassert>
#include <stdlib.h>
#include "pump_core/logger/pump_core_logger.h"
#include "pump_event/event_interface.h"
#include "pump_event/handle_event/handle_event_container.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

CHandleEventHashContainer::CHandleEventHashContainer() : m_pWork(&m_arrFd[0]), m_dwSize(0) {}

CHandleEventHashContainer::CHandleEventHashContainer(const CHandleEventHashContainer &) {}

CHandleEventHashContainer &CHandleEventHashContainer::operator=(const CHandleEventHashContainer &) { return *this; }

CUnidimEventContainer *CHandleEventHashContainer::begin()
{
    return &m_arrFd[0];
}

CUnidimEventContainer *CHandleEventHashContainer::end()
{
    return &m_arrFd[g_nFd];
}

const CUnidimEventContainer *CHandleEventHashContainer::cbegin() const
{
    return &m_arrFd[0];
}

const CUnidimEventContainer *CHandleEventHashContainer::cend() const
{
    return &m_arrFd[g_nFd];
}

bool CHandleEventHashContainer::anchor(CUnidimEventContainer *pWork)
{
    if (pWork < &m_arrFd[0] || pWork > &m_arrFd[g_nFd])
    {
        return false;
    }
    m_pWork = dynamic_cast<CHandleCap *>(pWork);
    return true;
}

CUnidimEventContainer *CHandleEventHashContainer::get()
{
    return m_pWork;
}

CUnidimEventContainer *CHandleEventHashContainer::next()
{
    return (m_pWork < &m_arrFd[g_nFd] ? ++m_pWork : &m_arrFd[g_nFd]);
}

const CUnidimEventContainer *CHandleEventHashContainer::cnext()
{
    return (m_pWork < &m_arrFd[g_nFd] ? ++m_pWork : &m_arrFd[g_nFd]);
}

CUnidimEventContainer *CHandleEventHashContainer::prior()
{
    return (m_pWork > &m_arrFd[0] ? --m_pWork : &m_arrFd[0]);
}

const CUnidimEventContainer *CHandleEventHashContainer::cprior()
{
    return (m_pWork > &m_arrFd[0] ? --m_pWork : &m_arrFd[0]);
}

bool CHandleEventHashContainer::insert(CUnidimEventContainer *pEvContainer, const Condition *pCond)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CFdEventHashContainer::insert() in");
#endif //_TEST_LEVEL_DEBUG
    CHandleCap *pWork = dynamic_cast<CHandleCap *>(pEvContainer);
    if ((size_t)pWork->m_pHandle->GetHandle() < 0 || (size_t)pWork->m_pHandle->GetHandle() > g_nFd
        || m_arrFd[(size_t)pWork->m_pHandle->GetHandle()].m_pHandle->GetHandle() > 0)
    {
        // TODO: 参数错误
        goto ret_bad;
    }
    m_arrFd[(size_t)pWork->m_pHandle->GetHandle()] = *pWork;
    ++m_dwSize;

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("insert fd == %ul",pWork->m_pHandle->GetHandle());
    PUMP_CORE_INFO("CFdEventHashContainer::insert() out with true");
#endif //_TEST_LEVEL_DEBUG
    return true;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("insert fd == %ul" , pWork->m_pHandle->GetHandle());
    PUMP_CORE_INFO("CFdEventHashContainer::insert() out with false");
#endif //_TEST_LEVEL_DEBUG
    return false;
}

bool CHandleEventHashContainer::insert(CEvent *pEv, const Condition *pCond)
{
    CHandleEvent *pFdEv = dynamic_cast<CHandleEvent *>(pEv);
    //if (pFdEv == NULL || pFdEv->m_fd < 0
    //    || m_arrFd[(size_t)pFdEv->m_fd].m_fd < 0)
    //{
    //    //TODO:参数无效
    //    return false;
    //}
    if (pFdEv == NULL || !pFdEv->m_pHandle->Available()
        || m_arrFd[(size_t)pFdEv->m_pHandle->GetHandle()].m_pHandle->GetHandle() < 0)
    {
        //TODO:参数无效
        return false;
    }

    if (this->m_arrFd[(size_t)pFdEv->m_fd].m_mapEv.find(pFdEv->m_emFdEvType) != this->m_arrFd[(size_t)pFdEv->m_fd].m_mapEv.end())
    {
        //TODO:err重复插入fd事件
        return false;
    }

    this->m_arrFd[(size_t)pFdEv->m_fd].m_mapEv.insert(
        CHandleCap::ItemType(pFdEv->m_emFdEvType, pFdEv));
    return true;
}

bool CHandleEventHashContainer::remove(CUnidimEventContainer *pEvContainer, const Condition *pCond)
{
    HandleEvent::CHandleCap *pWork = dynamic_cast<HandleEvent::CHandleCap *>(pEvContainer);
    if ((pWork->m_pHandle->GetHandle() < 0 || (size_t)pWork->m_pHandle->GetHandle() > g_nFd || m_arrFd[(size_t)pWork->m_pHandle->GetHandle()].m_pHandle->GetHandle() > 0)
        && (pWork->m_emState == HandleEvent::FdState::FD_STATE_CLOSED
        || pWork->m_emState == HandleEvent::FdState::FD_STATE_DEFAULT))
    {
        // 如若 pFDescriptor 正处于close状态则表示已经销毁
        // TODO: 无效的参数
        return false;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CFdEventHashContainer::remove() remove fd == %ul" , pWork->m_pHandle->GetHandle());
#endif //_TEST_LEVEL_DEBUG
    m_arrFd[(size_t)pWork->m_pHandle->GetHandle()].clear();
    --m_dwSize;
    return true;
}

bool CHandleEventHashContainer::remove(CEvent *pEv, const Condition *pCond)
{
    HandleEvent::CHandleEvent *pFEv = dynamic_cast<HandleEvent::CHandleEvent *>(pEv);
    if (pFEv == NULL || pFEv->m_pHandle->GetHandle() < 0
        || m_arrFd[(size_t)pFEv->m_fd].m_pHandle->GetHandle() < 0)
    {
        //TODO:参数无效
        return false;
    }

    if (m_arrFd[(size_t)pFEv->m_pHandle->GetHandle()].m_mapEv.find(pFEv->m_emFdEvType) == m_arrFd[(size_t)pFEv->m_pHandle->GetHandle()].m_mapEv.end())
    {
        //TODO:err试图删除未注册fd事件
        return false;
    }

    //由于使用boost::shared_ptr<CFdEvent>管理FdEvent,删除时无需认为释放内存
    m_arrFd[(size_t)pFEv->m_pHandle->GetHandle()].m_mapEv.erase(pFEv->m_emFdEvType);
    return true;
}

CUnidimEventContainer *CHandleEventHashContainer::find(const Condition *pCond)
{
    if (pCond == NULL || sizeof(::Pump::Event::HandleEvent::FindCond) != pCond->dwSize_)
    {
        //TODO: 参数错误
        return NULL;
    }
    const ::Pump::Event::HandleEvent::FindCond *pFCond = static_cast<const ::Pump::Event::HandleEvent::FindCond *>(pCond);
    if ((size_t)pFCond->fd_ < 0 || (size_t)pFCond->fd_ >= g_nFd
        /*|| m_arrFd[pFCond->m_fd].m_fd < 0*/)
    {
        //TODO: 查找条件无效
        return NULL;
    }
    return &m_arrFd[(size_t)pFCond->fd_];
}

bool CHandleEventHashContainer::clear()
{
    for (int i = 0; i < g_nFd; ++i)
    {
        m_arrFd[i].clear();
    }
    return true;
}

size_t CHandleEventHashContainer::size() const
{
    return m_dwSize;
}

//
CHandleEventMapContainer::CHandleEventMapContainer() : m_pWork(m_arrFd.begin()){}

CHandleEventMapContainer::CHandleEventMapContainer(const CHandleEventMapContainer &) {}

CHandleEventMapContainer &CHandleEventMapContainer::operator=(const CHandleEventMapContainer &) { return *this; }

CUnidimEventContainer *CHandleEventMapContainer::begin()
{
    if (m_arrFd.empty())
    {
        return PUMP_NULL;
    }
    return &((m_arrFd.begin())->second);
}

CUnidimEventContainer *CHandleEventMapContainer::end()
{
    if (m_arrFd.empty())
    {
        return PUMP_NULL;
    }
    return &((m_arrFd.end())->second);
}

const CUnidimEventContainer *CHandleEventMapContainer::cbegin() const
{
    if (m_arrFd.empty())
    {
        return PUMP_NULL;
    }
    return &((m_arrFd.begin())->second);
}

const CUnidimEventContainer *CHandleEventMapContainer::cend() const
{
    if (m_arrFd.empty())
    {
        return PUMP_NULL;
    }
    return &((m_arrFd.end())->second);
}

bool CHandleEventMapContainer::anchor(CUnidimEventContainer *pWork)
{
    if (!pWork)
    {
        return false;
    }
    CHandleCap * pHandleCap = static_cast<CHandleCap *>(pWork);
    IteratorType it = m_arrFd.find(pHandleCap->m_pHandle->GetHandle());
    if (it != m_arrFd.end())
    {
        return false;
    }
    m_pWork = it;
    return true;
}

CUnidimEventContainer *CHandleEventMapContainer::get()
{
    if (m_pWork==m_arrFd.end())
    {
        return PUMP_NULL;
    }
    return &(m_pWork->second);
}

CUnidimEventContainer *CHandleEventMapContainer::next()
{
    if (m_pWork == m_arrFd.end())
    {
        return PUMP_NULL;
    }
    ++m_pWork;
    return &(m_pWork->second);
}

const CUnidimEventContainer *CHandleEventMapContainer::cnext()
{
    if (m_pWork == m_arrFd.end())
    {
        return PUMP_NULL;
    }
    ++m_pWork;
    return &(m_pWork->second);
}

CUnidimEventContainer *CHandleEventMapContainer::prior()
{
    if (m_pWork == m_arrFd.begin() && m_arrFd.empty())
    {
        return PUMP_NULL;
    }
    --m_pWork;
    return &(m_pWork->second);
}

const CUnidimEventContainer *CHandleEventMapContainer::cprior()
{
    if (m_pWork == m_arrFd.begin() && m_arrFd.empty())
    {
        return PUMP_NULL;
    }
    --m_pWork;
    return &(m_pWork->second);
}

bool CHandleEventMapContainer::insert(CUnidimEventContainer *pEvContainer, const Condition *pCond)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CHandleEventMapContainer::insert() in");
#endif //_TEST_LEVEL_DEBUG
    CHandleCap *pWork = dynamic_cast<CHandleCap *>(pEvContainer);
    if (pWork == NULL
        || m_arrFd.find(pWork->m_pHandle->GetHandle())!=m_arrFd.end())
    {
        //TODO:参数无效或企图重复插入
        return false;
    }
    std::pair<IteratorType, bool> ret = m_arrFd.insert(ItemType(pWork->m_pHandle->GetHandle(), *pWork));
    if (!ret.second)
    {
        goto ret_bad;
    }

//ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("insert fd == %ul" , pWork->m_pHandle->GetHandle());
    PUMP_CORE_INFO("CHandleEventMapContainer::insert() out with true");
#endif //_TEST_LEVEL_DEBUG
    return true;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("insert fd == %ul" , pWork->m_pHandle->GetHandle());
    PUMP_CORE_INFO("CHandleEventMapContainer::insert() out with false" ) ;
#endif //_TEST_LEVEL_DEBUG
    return false;
}

bool CHandleEventMapContainer::insert(CEvent *pEv, const Condition *pCond)
{
    CHandleEvent *pFdEv = dynamic_cast<CHandleEvent *>(pEv);
    if (pFdEv == NULL || !pFdEv->m_pHandle->Available())
    {
        //TODO:参数无效
        return false;
    }
    IteratorType it = m_arrFd.find(pFdEv->m_pHandle->GetHandle());
    if (it==m_arrFd.end())
    {
        // 未找到对应句柄CHandleCap
        return false;
    }

    if (it->second.m_mapEv.find(pFdEv->m_emFdEvType) != it->second.m_mapEv.end())
    {
        //TODO:err重复插入fd事件
        return false;
    }

    it->second.m_mapEv.insert(
        CHandleCap::ItemType(pFdEv->m_emFdEvType, pFdEv));
    return true;
}

bool CHandleEventMapContainer::remove(CUnidimEventContainer *pEvContainer, const Condition *pCond)
{
    HandleEvent::CHandleCap *pWork = dynamic_cast<HandleEvent::CHandleCap *>(pEvContainer);
    if (pWork->m_pHandle->GetHandle() == PUMP_INVALID_HANDLE)
    {
        // 如若 pFDescriptor 正处于close状态则表示已经销毁
        // TODO: 无效的参数
        return false;
    }
    IteratorType it = m_arrFd.find(pWork->m_pHandle->GetHandle());
    if (it!=m_arrFd.end())
    {
        return false;
    }
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("CHandleEventMapContainer::remove() remove fd == %ul" , pWork->m_pHandle->GetHandle());
#endif //_TEST_LEVEL_DEBUG
    it->second.clear();
    if (it==m_pWork)
    {
        m_pWork = m_arrFd.erase(it);
    }
    m_arrFd.erase(it);
    return true;
}

bool CHandleEventMapContainer::remove(CEvent *pEv, const Condition *pCond)
{
    CHandleEvent *pFEv = dynamic_cast<CHandleEvent *>(pEv);
    if (pFEv == PUMP_NULL || !pFEv->m_pHandle->Available())
    {
        //TODO:参数无效
        return false;
    }
    IteratorType it = m_arrFd.find(pFEv->m_pHandle->GetHandle());
    if (it != m_arrFd.end())
    {
        // 未找到对应句柄CHandleCap
        return false;
    }

    //由于使用boost::shared_ptr<CFdEvent>管理FdEvent,删除时无需认为释放内存
    it->second.m_mapEv.erase(pFEv->m_emFdEvType);
    return true;
}

CUnidimEventContainer *CHandleEventMapContainer::find(const Condition *pCond)
{
    if (pCond == NULL || sizeof(::Pump::Event::HandleEvent::FindCond) != pCond->dwSize_)
    {
        //TODO: 参数错误
        return NULL;
    }
    const ::Pump::Event::HandleEvent::FindCond *pFCond = static_cast<const ::Pump::Event::HandleEvent::FindCond *>(pCond);
    if (pFCond->fd_ == PUMP_INVALID_HANDLE)
    {
        //TODO: 查找条件无效
        return NULL;
    }
    IteratorType it = m_arrFd.find(pFCond->fd_);
    if (it == m_arrFd.end())
    {
        return PUMP_NULL;
    }
    return &(it->second);
}

bool CHandleEventMapContainer::clear()
{
    m_arrFd.clear();
    return true;
}

size_t CHandleEventMapContainer::size() const
{
    return m_arrFd.size();
}

}
}
}