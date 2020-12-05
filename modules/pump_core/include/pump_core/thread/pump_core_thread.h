/**
 * @file pump_core_thread.h
 * @brief Library Header.
 * @details Declaration of thread control interface.
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

#ifndef PUMP_CORE_THREAD_H
#define PUMP_CORE_THREAD_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_object.h"

#if (defined PUMP_OS_WINDOWS)
typedef unsigned long int pump_thread_id;
#elif (defined PUMP_OS_POSIX)
typedef unsigned long int pump_thread_id;
#endif // (defined PUMP_OS_WINDOWS)

namespace Pump
{
namespace Core
{
namespace Thread
{

#define PUMP_INVALID_THREAD (-1)

class PUMP_CORE_CLASS CThread
{
public:
    typedef enum tagPUMP_THREAD_TYPE
    {
        PUMP_THREAD_DEFAULT = 0,
        PUMP_THREAD_BUILDIN,
        PUMP_THREAD_BOOST,
        PUMP_THREAD_ACE,
    } PUMP_THREAD_TYPE;
public:
    CThread();
    CThread(pump_void_t * pData);
    explicit CThread(PUMP_THREAD_TYPE emType, pump_void_t * pData);
    virtual ~CThread();
    virtual pump_int32_t Start();
    virtual pump_int32_t Stop();
    virtual pump_int32_t Suspend();
    virtual pump_int32_t Resume();
    virtual pump_int32_t  SetPriority(pump_int32_t iPriority);
    virtual pump_int32_t  SetSchedPolicy(pump_int32_t iSchedPolicy);
    pump_thread_id GetTID();
    void SetData(void * pData);
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData) = 0;
    static pump_void_t * PUMP_CALLBACK __ThreadCallbackCore(pump_void_t * pData);
private:
    pump_void_t * m_pData;
    ::Pump::Core::CPrimitiveBase * m_pPrimitive;
    PUMP_THREAD_TYPE m_emType;
};

}
}
}

/**
 * PUMP_CORE_Thread_Create create thread.
 * @param StartAddress (IN) thread body
 * @param Params (IN) parameter input into thread body.
 * @param StackSize (IN) thread body stack size.
 * @param IsSuspend (IN) is thread suspend after create, only valid in windows.
 * @param Priority (IN) thread priority
 * @param SchedPolicy (IN) thread sched policy
 * @return NULL fail, else success.
 * @sa PUMP_CORE_ThreadDetached_Create(),PUMP_CORE_Thread_Wait()
 */
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_Thread_Create(pump_pvoid_t(PUMP_CALLBACK *StartAddress)(pump_pvoid_t), pump_void_t* Params, pump_uint32_t StackSize, \
    pump_bool_t IsSuspend = PUMP_FALSE, pump_int32_t Priority = 0, pump_int32_t SchedPolicy = 0);

/**
 * PUMP_CORE_ThreadDetached_Create create thread with detached attr.
 * @param StartAddress (IN) thread body
 * @param Params (IN) parameter input into thread body.
 * @param StackSize (IN) thread body stack size.
 * @return PUMP_CORE_TRUE success, PUMP_FALSE fail
 * @sa PUMP_CORE_Thread_Create()
 */
PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_ThreadDetached_Create(pump_pvoid_t(PUMP_CALLBACK *StartAddress)(pump_pvoid_t), pump_void_t* Params, pump_uint32_t StackSize);

/**
 * PUMP_CORE_Thread_Wait wait thread and release resource.
 * @param hThreadHandle (IN) thread handle, created by PUMP_CORE_Thread_Create
 * @return 0 success, -1 fail
 * @sa PUMP_CORE_Thread_Create()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Wait(pump_handle_t hThreadHandle);

/**
 * PUMP_CORE_Thread_Suspend suspend thread
 * @param hThreadHandle (IN) thread handle, created by PUMP_CORE_Thread_Create
 * @return 0 success, -1 fail
 * @sa PUMP_CORE_Thread_Create()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Suspend(pump_handle_t hThreadHandle);

/**
 * PUMP_CORE_Thread_Resume resume thread, only valid in windows
 * @param hThreadHandle (IN) thread handle, created by PUMP_CORE_Thread_Create
 * @return 0 success, -1 fail
 * @sa PUMP_CORE_Thread_Create()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Resume(pump_handle_t hThreadHandle);

/**
 * PUMP_CORE_Thread_SetPriority set thread priority
 * @param hThreadHandle (IN) thread handle, created by PUMP_CORE_Thread_Create
 * @param Priority (IN) sched priority
 * @return 0 success, -1 fail
 * @sa PUMP_CORE_Thread_Create()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_SetPriority(pump_handle_t hThreadHandle, pump_int32_t Priority);

/**
 * PUMP_CORE_Thread_SetSchedPolicy set thread sched policy
 * @param hThreadHandle (IN) thread handle, created by PUMP_CORE_Thread_Create
 * @param SchedPolicy (IN) sched policy
 * @return 0 success, -1 fail
 * @sa PUMP_CORE_Thread_Create()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_SetSchedPolicy(pump_handle_t hThreadHandle, pump_int32_t SchedPolicy);

/**
 * PUMP_CORE_Thread_Exit exit thread.
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_ThreadDetached_Create()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Exit();

/**
 * PUMP_CORE_Thread_GetSelfId
 * @param void
 * @return thread ID
 */
PUMP_CORE_API pump_thread_id PUMP_CALLBACK PUMP_CORE_Thread_GetSelfId();


//////////////////////////////////////////////////////////////////////////
// Thread Local Storage API.
//////////////////////////////////////////////////////////////////////////
#define PUMP_INVALID_TLS (pump_handle_t)(-1)

/**
 * PUMP_CORE_ThreadTls_Create
 * @param void
 * @return thread local storage handle.
 */
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadTls_Create();
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadTls_SetValue(pump_handle_t hTls, pump_pvoid_t pVal);
PUMP_CORE_API pump_pvoid_t PUMP_CALLBACK PUMP_CORE_ThreadTls_GetValue(pump_handle_t hTls);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadTls_Destroy(pump_handle_t hTls);

#endif //PUMP_CORE_THREAD_H
