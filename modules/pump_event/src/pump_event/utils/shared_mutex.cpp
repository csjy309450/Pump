//
// Created by yz on 20-2-16.
//

#include "pump_event/utils/shared_mutex.h"

namespace Pump
{
namespace Event
{

CSharedMutex::CSharedMutex()
: m_csThis(Pump::Memory::PUMP_NULLPTR)
{
    m_csThis.construct();
}

CSharedMutex::~CSharedMutex(){}

CSharedMutex::CSharedMutex(const CSharedMutex& other)
 : m_csThis(other.m_csThis) {}

CSharedMutex &CSharedMutex::operator=(const CSharedMutex& other)
{
    m_csThis = (other.m_csThis);
    return *this;
}

void CSharedMutex::Lock()
{
    m_csThis->Lock();
}

bool CSharedMutex::TryLock()
{
    return m_csThis->TryLock();
}

void CSharedMutex::Unlock()
{
    m_csThis->Unlock();
}

}
}