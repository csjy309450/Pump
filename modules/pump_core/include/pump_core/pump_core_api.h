/**
 * @file pump_core_api.h
 * @brief Library Header.
 * @details Declaration of global interface.
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

#ifndef PUMP_CORE_API_H
#define PUMP_CORE_API_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/network/pump_core_addr.h"

// common macro funtion
#ifndef PUMP_CORE_Max
#define PUMP_CORE_Max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef PUMP_CORE_Min
#define PUMP_CORE_Min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Init();
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Cleanup();
PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_SetSystemError(pump_int32_t iError);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetSystemError();
PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_Sleep(pump_int32_t iMilliseconds);
PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_Assert();

#endif // PUMP_CORE_API_H