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

#ifndef PUMP_CORE_THREADPOOL_H    
#define PUMP_CORE_THREADPOOL_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/os_wrapper/pump_core_os_thread_pool.h"
#include "pump_core/pump_core_noncopyable.h"

namespace Pump
{
namespace Core
{
namespace Thread
{

class PUMP_CORE_CLASS  CThreadPool 
    : public ::Pump::Core::CNonCopyable
{
public:
    CThreadPool(pump_void_t);
    ~CThreadPool(pump_void_t);

public:
    pump_int32_t Create(pump_uint32_t InitThreadNum, pump_uint32_t MaxThreadNum);
    pump_int32_t Destroy(pump_void_t);
    pump_int32_t Work(PUMP_CORE_ThreadPool_WorkRoutine WorkRoutine, pump_pvoid_t Params);

private:
    pump_handle_t m_threadpoolhandle;
};

}
}
}

#endif // PUMP_CORE_THREADPOOL_H
