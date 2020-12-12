//
// Created by yz on 19-5-26.
//

#ifndef PUMP_EVENT_FDEVCONTAINER_H
#define PUMP_EVENT_FDEVCONTAINER_H
#include "pump_macro/pump_pre.h"
#include <cstddef>
#include "pump_core/network/pump_core_sock.h"
#include "pump_event/pump_event_config.h"
#include "pump_event/handle_event/handle_event.h"
#include "pump_event/handle_event/handle_cap.h"
#include "pump_event/event_container.h"

namespace Pump
{
namespace Event
{
namespace HandleEvent
{

const size_t g_nFd = FD_SETSIZE; //TODO:根据文件描述符的宏来定义

class CHandleEvent;
class CHandleCap;

struct FindCond
    : public Condition
{
    pump_fd_t fd_;
};

class PUMP_EVENT_CLASS CHandleEventHashContainer
    : public CMultidimEventContainer
{
public:
    CHandleEventHashContainer();

    virtual CUnidimEventContainer *begin();

    virtual CUnidimEventContainer *end();

    virtual const CUnidimEventContainer *cbegin() const;

    virtual const CUnidimEventContainer *cend() const;

    virtual bool anchor(CUnidimEventContainer *pWork);

    virtual CUnidimEventContainer *get();

    virtual CUnidimEventContainer *next();

    virtual const CUnidimEventContainer *cnext();

    virtual CUnidimEventContainer *prior();

    virtual const CUnidimEventContainer *cprior();

    virtual bool insert(CUnidimEventContainer *pEvContainer, const Condition *pCond = NULL);

    virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);

    virtual bool remove(CUnidimEventContainer *pEvContainer, const Condition *pCond = NULL);

    virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);

    virtual CUnidimEventContainer *find(const Condition *pCond = NULL);

    virtual bool clear();

    virtual size_t size() const;

#ifdef _TEST_LEVEL_DEBUG

    CHandleCap *get_arr_fd() { return m_arrFd; }

#endif //_TEST_LEVEL_DEBUG

private:
    CHandleEventHashContainer(const CHandleEventHashContainer &);

    CHandleEventHashContainer &operator=(const CHandleEventHashContainer &);

private:
    CHandleCap m_arrFd[g_nFd + 1]; //!< fd hash数组
    CHandleCap *m_pWork;
    size_t m_dwSize;
};

//TODO 20200314 因为windows下fd_set的实现机制不同，windows下不是hash，需要实现map的容器
class PUMP_EVENT_CLASS CHandleEventMapContainer
    : public CMultidimEventContainer
{
public:
    typedef std::map<pump_handle_t, CHandleCap> ContainerType;
    typedef ContainerType::iterator IteratorType;
    typedef std::pair<pump_handle_t, CHandleCap> ItemType;
public:
    CHandleEventMapContainer();

    virtual CUnidimEventContainer *begin();

    virtual CUnidimEventContainer *end();

    virtual const CUnidimEventContainer *cbegin() const;

    virtual const CUnidimEventContainer *cend() const;

    virtual bool anchor(CUnidimEventContainer *pWork);

    virtual CUnidimEventContainer *get();

    virtual CUnidimEventContainer *next();

    virtual const CUnidimEventContainer *cnext();

    virtual CUnidimEventContainer *prior();

    virtual const CUnidimEventContainer *cprior();
    // 插入句柄CHandleCap
    virtual bool insert(CUnidimEventContainer *pEvContainer, const Condition *pCond = NULL);
    // 插入事件
    virtual bool insert(CEvent *pEv, const Condition *pCond = NULL);

    virtual bool remove(CUnidimEventContainer *pEvContainer, const Condition *pCond = NULL);

    virtual bool remove(CEvent *pEv, const Condition *pCond = NULL);

    virtual CUnidimEventContainer *find(const Condition *pCond = NULL);

    virtual bool clear();

    virtual size_t size() const;

private:
    CHandleEventMapContainer(const CHandleEventMapContainer &);

    CHandleEventMapContainer &operator=(const CHandleEventMapContainer &);

private:
    ContainerType m_arrFd; //!< fd 容器
    IteratorType m_pWork;
};

}
}
}

#endif //PUMP_EVENT_FDEVCONTAINER_H
