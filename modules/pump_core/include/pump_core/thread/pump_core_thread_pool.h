#ifndef PUMP_CORE_THREADPOOL_H	
#define PUMP_CORE_THREADPOOL_H

#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_noncopyable.h"

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

typedef pump_pvoid_t(PUMP_CALLBACK *PUMP_CORE_ThreadPool_WorkRoutine)(pump_pvoid_t);

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

#if (defined PUMP_COMPILER_CXX)
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

#endif // (defined PUMP_COMPILER_CXX)
#endif // PUMP_CORE_THREADPOOL_H
