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

#ifndef PUMP_CORE_OS_SEMAPHORE_H
#define PUMP_CORE_OS_SEMAPHORE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"

#if defined (PUMP_OS_WINDOWS)
typedef pump_handle_t pump_sem_t;
#elif defined(PUMP_OS_POSIX)
#include <semaphore.h>
#   if defined (PUMP_OS_APPLE)
typedef sem_t* pump_sem_t;
#   else
typedef sem_t pump_sem_t;
#   endif // defined (PUMP_OS_APPLE)
#else
#error os not supported.
#endif // defined (PUMP_OS_WINDOWS)

/**
 * PUMP_CORE_SemCreate
 * @param pSem (OUT) sem want to create.
 * @param nInitCount (IN) init resources after create.
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_SemDestroy(),PUMP_CORE_SemWait(),PUMP_CORE_SemTimedWait(),PUMP_CORE_SemPost()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemCreate(pump_sem_t* pSem, pump_uint32_t nInitCount);

/**
 * PUMP_CORE_SemDestroy
 * @param pSem (IN) created by PUMP_CORE_SemCreate
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_SemCreate(),PUMP_CORE_SemWait(),PUMP_CORE_SemTimedWait(),PUMP_CORE_SemPost()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemDestroy(pump_sem_t* pSem);

/**
 * PUMP_CORE_SemWait
 * @param pSem (IN) created by PUMP_CORE_SemCreate
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_SemCreate(),PUMP_CORE_SemDestroy(),PUMP_CORE_SemTimedWait(),PUMP_CORE_SemPost()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemWait(pump_sem_t* pSem);

/**
 * PUMP_CORE_SemTimedWait
 * @param pSem (IN) created by PUMP_CORE_SemCreate
 * @param nTimeOut (IN) wait timeout.
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_SemCreate(),PUMP_CORE_SemDestroy(),PUMP_CORE_SemWait(),PUMP_CORE_SemPost()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemTimedWait(pump_sem_t* pSem, pump_uint32_t nTimeOut);

/**
 * PUMP_CORE_SemPost
 * @param pSem (IN) created by PUMP_CORE_SemCreate
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_SemCreate(),PUMP_CORE_SemDestroy(),PUMP_CORE_SemWait(),PUMP_CORE_SemTimedWait()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemPost(pump_sem_t* pSem);

#endif // PUMP_CORE_OS_SEMAPHORE_H