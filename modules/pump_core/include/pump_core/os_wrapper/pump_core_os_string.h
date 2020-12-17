/**
 * @file pump_core_string.h
 * @brief Library Header.
 * @details Declaration of string interface.
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

#ifndef PUMP_CORE_OS_STRING_H
#define PUMP_CORE_OS_STRING_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"

PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_Printf(const pump_char_t * szFormate, ...);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_StrCaseCmp(const char *s1, const char *s2);
PUMP_CORE_API char * PUMP_CALLBACK PUMP_CORE_StrtOk_R(char *s, const char *tokens, char **lasts);
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CharIsAlpha(pump_char_t c);
PUMP_CORE_API int PUMP_CALLBACK PUMP_CORE_CharIsSpace(pump_char_t c);

#endif // PUMP_CORE_OS_STRING_H
