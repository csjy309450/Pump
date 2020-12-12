//
// Created by yz on 20-2-16.
//

#ifndef PUMP_EVENT_SHARED_RWLOCKER_H
#define PUMP_EVENT_SHARED_RWLOCKER_H
#include "pump_core/thread/pump_core_rwlocker.h"
#include "pump_memory/smart_ptr/sharedptr.hpp"
#include "pump_event/pump_event_config.h"

namespace Pump
{
namespace Event
{

class PUMP_EVENT_CLASS CSharedRWLocker
{
public:
    typedef Pump::Memory::SharedPtr<Pump::Core::Thread::CRWLocker> RWLockerType;
public:
    CSharedRWLocker();
    
    ~CSharedRWLocker();
    
    CSharedRWLocker(const CSharedRWLocker &other);
    
    CSharedRWLocker &operator=(const CSharedRWLocker &other);
    
    void ReadLock();
    
    void ReadUnlock();
    
    void WriteLock();
    
    void WriteUnlock();

private:
    RWLockerType m_csThis;
};

}
}

#endif //PUMP_EVENT_SHARED_RWLOCKER_H
