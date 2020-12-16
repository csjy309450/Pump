/**
 * @file pump_core_semaphore.h
 * @brief Library Header.
 * @details Declaration of semaphore interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-12-01
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-12-01<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#ifndef PUMP_CORE_SEMAPHORE_H
#define PUMP_CORE_SEMAPHORE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/os_wrapper/pump_core_os_semaphore.h"

namespace Pump
{
namespace Core
{
namespace Thread
{

class PUMP_CORE_CLASS CSemaphore
{
public:
    CSemaphore(pump_uint32_t nInitCount, pump_uint32_t nMaxCount);
    ~CSemaphore();

public:
    pump_int32_t Wait();
    pump_int32_t TimedWait(pump_uint32_t nTimeOut);
    pump_int32_t Post();

private:
    pump_sem_t m_sem;
#ifdef PUMP_OS_POSIX
    pump_uint32_t m_nMaxCount;
    CMutex m_csLock;
#   ifdef PUMP_OS_APPLE
    pump_uint32_t m_nCurCount;
#   endif // PUMP_OS_APPLE
#endif // PUMP_OS_POSIX
};

}
}
}

#endif // PUMP_CORE_SEMAPHORE_H