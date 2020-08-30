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
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_object.h"
//using namespace boost;

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

#define PUMP_INVALID_THREAD -1

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
    CThread(PUMP_THREAD_TYPE emType, pump_void_t * pData);
    virtual ~CThread();
    virtual pump_int32_t Start();
    virtual pump_int32_t Stop();
    pump_thread_id GetTID();
    void SetData(void * pData);
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData) = 0;
    static pump_void_t * PUMP_CALLBACK __ThreadCallbackCore(pump_void_t * pData);
private:
    pump_void_t * m_pData;
    ::Pump::Core::__CPrimitiveBase * m_pPrimitive;
    PUMP_THREAD_TYPE m_emType;
};

}
}
}

#endif //PUMP_CORE_THREAD_H
