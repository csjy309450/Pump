/**
 * @file pump_core_thread_pool.h
 * @brief Library Header.
 * @details Declaration of thread pool control interface.
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

#ifndef PUMP_CORE_OS_THREADPOOL_H    
#define PUMP_CORE_OS_THREADPOOL_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"

#define PUMP_INVALID_THREADPOOL NULL

PUMP_CORE_API pump_int32_t  PUMP_CALLBACK PUMP_CORE_InitThreadPool();
PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_FiniThreadPool();

/**
 * PUMP_CORE_ThreadPool_Create
 * @param InitThreadNum (IN) init thread number.
 * @param MaxThreadNum (IN) max thread number.
 * @return NULL fail, ese success.
 * @sa PUMP_CORE_ThreadPool_Work(),PUMP_CORE_ThreadPool_Destroy()
 */
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadPool_Create(pump_uint32_t InitThreadNum, pump_uint32_t MaxThreadNum);
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadPool_CreateEx(pump_uint32_t InitThreadNum, pump_uint32_t MaxThreadNum, pump_uint32_t StackSize);
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadPool_CreateFlex(pump_uint32_t InitThreadNum, pump_uint32_t MaxThreadNum,
    pump_uint32_t StackSize, pump_uint32_t TimeOut);

/**
 * PUMP_CORE_ThreadPool_Destroy
 * @param ThreadPoolHandle (IN) created by PUMP_CORE_ThreadPool_Create
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_ThreadPool_Create(),PUMP_CORE_ThreadPool_Work()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadPool_Destroy(pump_handle_t ThreadPoolHandle);

typedef pump_pvoid_t(PUMP_CALLBACK * PUMP_CORE_ThreadPool_WorkRoutine)(pump_pvoid_t);

/**
 * PUMP_CORE_ThreadPool_Work
 * @param ThreadPoolHandle (IN) created by PUMP_CORE_ThreadPool_Create
 * @param WorkRoutine (IN) do the job function.
 * @param Params (IN) parameter pass to WorkRoutine.
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_ThreadPool_Create(),PUMP_CORE_ThreadPool_Destroy()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadPool_Work(pump_handle_t ThreadPoolHandle, PUMP_CORE_ThreadPool_WorkRoutine WorkRoutine, pump_pvoid_t Params);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadPool_WorkEx(pump_handle_t ThreadPoolHandle, PUMP_CORE_ThreadPool_WorkRoutine WorkRoutine, pump_pvoid_t Params, pump_bool_t bWaitForIdle);

#endif // PUMP_CORE_OS_THREADPOOL_H
