/**
 * @file __pump_core_inner_logger.h
 * @brief Library Inner Header.
 * @details Declaration of pump_core inner loger interface.
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

#ifndef __PUMP_CORE_INNER_LOGGER
#define __PUMP_CORE_INNER_LOGGER
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/__pump_core_global_ctrl.h"
#include "pump_core/logger/pump_core_logger.h"

#define __PUMP_CORE_INFO(form_, ...) ::Pump::Core::CPumpCoreLogGuide().WriteLine(PUMP_LOG_INFO, __FILE__,__LINE__, form_,__VA_ARGS__)
#define __PUMP_CORE_WARING(form_, ...) ::Pump::Core::CPumpCoreLogGuide().WriteLine(PUMP_LOG_WARNING, __FILE__,__LINE__, form_,__VA_ARGS__)
#define __PUMP_CORE_ERR(form_, ...) ::Pump::Core::CPumpCoreLogGuide().WriteLine(PUMP_LOG_ERROR, __FILE__,__LINE__, form_,__VA_ARGS__)
#define __PUMP_CORE_LOG_ASSERT(_exp,form_, ...) assert(_exp);__PUMP_CORE_INFO(form_, __VA_ARGS__)

#endif // __PUMP_CORE_INNER_LOGGER

