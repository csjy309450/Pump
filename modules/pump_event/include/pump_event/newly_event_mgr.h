//
// Created by yz on 19-5-26.
//
#ifndef PUMP_EVENT_NEWLYEVENTMGR_H
#define PUMP_EVENT_NEWLYEVENTMGR_H
#include <list>
#include "pump_event/pump_event_config.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_memory/smart_buffer/circular_buffer_interface.hpp"

//class CNewlyEvMgrGuiderForInner;
//class CNewlyEvMgrGuiderForUser;

namespace Pump
{
namespace Event
{

class CEvent;

/**
 * @brief 新增事件管理器
 */
class PUMP_EVENT_CLASS CNewlyEventMgr
    : public ::Pump::Core::CNonCopyable
    , public ::Pump::Memory::circular_buffer_interface< std::list< CEvent* > >
{
public:
    typedef std::list<CEvent *> ContainerType;

    typedef ContainerType::iterator ContainerIteratorType;

    typedef ::Pump::Memory::circular_buffer_interface< std::list< CEvent* > >::buf_iterator BufferIteratorType;

    friend class CNewlyEvMgrGuiderForInner;

    friend class CNewlyEvMgrGuiderForUser;

public:
    CNewlyEventMgr();

    ~CNewlyEventMgr();

private:
    bool insert(CEvent *pEv);

    virtual BufferIteratorType get_offscreen_unlocked();

    virtual void swap();

private:
    std::list<CEvent *> m_listNewlyEv_0; //! Newly态事件双缓冲容器. 容器非Event所有者,不允许释放
    std::list<CEvent *> m_listNewlyEv_1; //! Newly态事件双缓冲容器. 容器非Event所有者,不允许释放
    ::Pump::Core::Thread::CMutex m_csOffscreen; //! 用于对Offscreen缓冲区的操作加锁
    // TODO: 增加挂起态事件容器
};

class PUMP_EVENT_CLASS CNewlyEvMgrGuiderForInner
    : public ::Pump::Core::CNonCopyable
{
public:
    explicit CNewlyEvMgrGuiderForInner(CNewlyEventMgr &);

    bool insert(CEvent *pEv);

    void swap();

    CNewlyEventMgr::BufferIteratorType get_work_locked();

    void unlock_work();

private:
    CNewlyEventMgr &m_refNewlyEvMgr;
};

class PUMP_EVENT_CLASS CNewlyEvMgrGuiderForUser
{
public:
    explicit CNewlyEvMgrGuiderForUser(CNewlyEventMgr &);

    bool insert(CEvent *pEv);

#ifdef _TEST_LEVEL_DEBUG

    CNewlyEventMgr::BufferIteratorType get_offscreen_locked();

    void unlock_offscreen();

#endif //_TEST_LEVEL_DEBUG

private:
    CNewlyEventMgr &m_refNewlyEvMgr;
};

}
}

#endif //PUMP_EVENT_NEWLYEVENTMGR_H
