/**
 * @file pump_core_api.cpp
 * @brief Library Implementation.
 * @details Implementation of global interface.
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

#include "pump_macro/pump_pre.h"
#include "pump_core/__pump_core_global_ctrl.h"
#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/pump_core_addr.h"
#ifdef PUMP_OS_WINDOWS
#include <synchapi.h>
#elif (defined PUMP_OS_POSIX)
#include <unistd.h>
#endif // PUMP_OS_WINDOWS
//
//PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Init()
//{
//    if (::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
//    {
//        return PUMP_OK;
//    }
//    if (::Pump::Core::__CPumpCoreGlobalCtrl::Create() != PUMP_OK)
//    {
//        return PUMP_ERROR;
//    }
//    return ::Pump::Core::__CPumpCoreGlobalCtrl::Init();
//}
//
//PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Cleanup()
//{
//    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
//    {
//        return PUMP_OK;
//    }
//    if (::Pump::Core::__CPumpCoreGlobalCtrl::Cleanup() != PUMP_OK)
//    {
//        return PUMP_ERROR;
//    }
//
//    return ::Pump::Core::__CPumpCoreGlobalCtrl::Destroy();
//}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetSystemError()
{
#ifdef PUMP_OS_WINDOWS
    return ::GetLastError();
#elif (defined PUMP_OS_POSIX)
    return error;
#endif // PUMP_OS_WINDOWS
}

PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_SetSystemError(pump_int32_t iError)
{
#ifdef PUMP_OS_WINDOWS
    ::SetLastError(iError);
#elif (defined PUMP_OS_POSIX)
    erron = iError;
#endif // PUMP_OS_WINDOWS
}

PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_Sleep(pump_int32_t iMilliseconds)
{
#ifdef PUMP_OS_WINDOWS
    ::Sleep(iMilliseconds);
#elif (defined PUMP_OS_POSIX)
    ::usleep(iMilliseconds);
#endif // PUMP_OS_WINDOWS
}