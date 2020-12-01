#ifndef __PUMP_CORE_INNER_LOGGER
#define __PUMP_CORE_INNER_LOGGER
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core\__pump_core_global_ctrl.h"
#include "pump_core/logger/pump_core_logger.h"

#define __PUMP_CORE_INFO ::Pump::Core::CLogGuide(::Pump::Core::__CPumpCoreGlobalCtrl::GetLogger(), __FILE__,__LINE__,PUMP_LOG_INFO)
#define __PUMP_CORE_WARING ::Pump::Core::CLogGuide(::Pump::Core::__CPumpCoreGlobalCtrl::GetLogger(),__FILE__,__LINE__,PUMP_LOG_WARNING)
#define __PUMP_CORE_ERR ::Pump::Core::CLogGuide(::Pump::Core::__CPumpCoreGlobalCtrl::GetLogger(),__FILE__,__LINE__,PUMP_LOG_ERROR)
#define __PUMP_CORE_LOG_ASSERT(_exp) assert(_exp);__PUMP_CORE_INFO

#endif // __PUMP_CORE_INNER_LOGGER

