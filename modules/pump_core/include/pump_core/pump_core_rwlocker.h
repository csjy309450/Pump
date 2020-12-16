/**
 * @file pump_core_rwlocker.h
 * @brief Library Header.
 * @details Declaration of rw-locker interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#ifndef PUMP_CORE_RWLOCK_H
#define PUMP_CORE_RWLOCK_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/pump_core_atomic_op.hpp"
#include "pump_core/pump_core_mutex.h"

namespace Pump
{
namespace Core
{
namespace Thread
{

//PUMP_TEMP_EXP(Pump::Core::Thread::CAtomicOpGuilder2, <unsigned int>);
//PUMP_TEMP_EXP(Pump::Core::Thread::CAtomicOpGuilder2, <bool>);

class PUMP_CORE_CLASS CRWLocker
: public Pump::Core::CNonCopyable
{
public:

public:
    CRWLocker();
    
    ~CRWLocker();
    
    void readLock();
    
    void readUnlock();
    
    void writeLock();
    
    void writeUnlock();
private:
    CMutex m_muxRead;
    CMutex m_muxWrite;
    CAtomicOpGuilder2<unsigned int> m_nReader;
    CAtomicOpGuilder2<bool> m_wFalg;
};

}
}
}

#endif //PUMP_PUMP_CORE_RWLOCK_H
