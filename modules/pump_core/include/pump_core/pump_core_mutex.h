/**
 * @file pump_core_mutex.h
 * @brief Library Inner Header.
 * @details Declaration of mutex control interface.
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

#ifndef PUMP_CORE_MUTEX_H
#define PUMP_CORE_MUTEX_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/os_wrapper/pump_core_os_mutex.h"
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_object.h"

namespace Pump
{
namespace Core
{
namespace Thread
{

class PUMP_CORE_CLASS CMutex
    : public CObjectBase
{
public:
    CMutex();
    virtual ~CMutex();
    void Lock();
    bool TryLock();
    void Unlock();
private:
    CMutex(const CMutex & other);
};

class PUMP_CORE_CLASS CMutexGuilder
{
public:
    CMutexGuilder(CMutex & locker);
    ~CMutexGuilder();
private:
    CMutexGuilder();
    CMutexGuilder(const CMutexGuilder &);
private:
    CMutex & m_locker;
};

PUMP_CORE_EXP void PUMP_CALLBACK MutexLocker(CMutex &mux0, CMutex &mux1);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5, CMutex &mux6);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5, CMutex &mux6);

}
}
}

#endif // PUMP_CORE_MUTEX_H
