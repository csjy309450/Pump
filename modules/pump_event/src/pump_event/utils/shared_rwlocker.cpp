//
// Created by yz on 20-2-16.
//

#include "pump_event/utils/shared_rwlocker.h"
#include "pump_memory/pump_memory_def.h"

namespace Pump
{
namespace Event
{
CSharedRWLocker::CSharedRWLocker(): m_csThis(Pump::Memory::PUMP_NULLPTR)
{
    m_csThis.construct();
}

CSharedRWLocker::~CSharedRWLocker(){}

CSharedRWLocker::CSharedRWLocker(const CSharedRWLocker &other)
: m_csThis(other.m_csThis){}

CSharedRWLocker &CSharedRWLocker::operator=(const CSharedRWLocker &other)
{
    m_csThis = (other.m_csThis);
    return *this;
}

void CSharedRWLocker::ReadLock()
{
    m_csThis->readLock();
}

void CSharedRWLocker::ReadUnlock()
{
    m_csThis->readUnlock();
}

void CSharedRWLocker::WriteLock()
{
    m_csThis->writeLock();
}

void CSharedRWLocker::WriteUnlock()
{
    m_csThis->writeUnlock();
}
}
}