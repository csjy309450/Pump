/**
 * @file pump_core_global_resource_mgr.h
 * @brief Library Header.
 * @details Declaration of global resource interface.
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

#ifndef PUMP_CORE_GLOBAL_RESOURCE_MGR_H
#define PUMP_CORE_GLOBAL_RESOURCE_MGR_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/logger/pump_core_logger.h"

#ifdef PUMP_COMPILER_CXX
namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CGlobalResourceMgr
  : public CNonCopyable
{
public:
    CGlobalResourceMgr() {}
    virtual ~CGlobalResourceMgr() {}
};

}
}
#endif // PUMP_COMPILER_CXX

#endif //PUMP_CORE_GLOBAL_RESOURCE_MGR_H
