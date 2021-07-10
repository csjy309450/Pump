/**
 * @file pump_core_config.h
 * @brief Library Header.
 * @details Declaration of pump_core global config macro.
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

#ifndef PUMP_CORE_CONFIG_H
#define PUMP_CORE_CONFIG_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"

// pump_core version info
#ifndef PUMP_CORE_VERSION_MAJOR
#   define PUMP_CORE_VERSION_MAJOR 0
#endif // !PUMP_CORE_VERSION_MAJOR
#ifndef PUMP_CORE_VERSION_MINOR
#   define PUMP_CORE_VERSION_MINOR 0
#endif // !PUMP_CORE_VERSION_MINOR
#ifndef PUMP_CORE_VERSION_PATCH
#   define PUMP_CORE_VERSION_PATCH 9
#endif // !PUMP_CORE_VERSION_PATCH
#ifndef PUMP_CORE_VERSION_BUILD
#   define PUMP_CORE_VERSION_BUILD 0
#endif // !PUMP_CORE_VERSION_BUILD
#define __PUMP_CORE_VERSION_STR(str_) #str_
#define __PUMP_CORE_VERJOIN(major_,minor_,patch_,build_) __PUMP_CORE_VERSION_STR(major_.minor_.patch_.build_)
#define PUMP_CORE_VERSION __PUMP_CORE_VERJOIN(PUMP_CORE_VERSION_MAJOR,PUMP_CORE_VERSION_MINOR,PUMP_CORE_VERSION_PATCH,PUMP_CORE_VERSION_BUILD)

#endif // PUMP_CORE_CONFIG_H
