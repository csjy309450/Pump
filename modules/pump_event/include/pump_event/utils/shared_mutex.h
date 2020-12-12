//
// Created by yz on 20-2-16.
//

#ifndef PUMP_EVENT_SHARED_MUTEX_H
#define PUMP_EVENT_SHARED_MUTEX_H
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_memory/smart_ptr/sharedptr.hpp"
#include "pump_event/pump_event_config.h"

namespace Pump
{
namespace Event
{

class PUMP_EVENT_CLASS CSharedMutex
{
public:
    typedef Pump::Memory::SharedPtr<Pump::Core::Thread::CMutex> MutexType;
public:
    CSharedMutex();
    ~CSharedMutex();
    CSharedMutex(const CSharedMutex& other);
    CSharedMutex &operator=(const CSharedMutex& other);
    void Lock();
    bool TryLock();
    void Unlock();
private:
    MutexType m_csThis;
};

}
}

#endif //PUMP_EVENT_SHARED_MUTEX_H
