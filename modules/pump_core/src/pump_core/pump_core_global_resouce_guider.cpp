/**
 * @file pump_core_global_resouce_guider.cpp
 * @brief Library Implementation.
 * @details Implementation of global resource guider.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-12-01
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-12-05<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#include "pump_core/pump_core_global_resouce_guider.hpp"
#include "pump_core/pump_core_global_ctrl_base.h"

namespace Pump
{
namespace Core
{

PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadLock()
{
    CGlobalCtrlBase::ReadLock();
}

PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadUnlock()
{
    CGlobalCtrlBase::ReadUnlock();
}

}
}