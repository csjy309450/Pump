/**
* @file pump_core_global_mgr.h
* @brief Library Inner Header.
* @details Declaration of base global resource control interface.
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

#ifndef PUMP_DRONE_GLOBAL_CTRL_H
#define PUMP_DRONE_GLOBAL_CTRL_H

#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/logger/pump_core_logger.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CGlobalManagement
  : public CNonCopyable
{
public:
  CGlobalManagement() {}
  virtual ~CGlobalManagement() {}
};

}
}

#endif //PUMP_DRONE_GLOBAL_CTRL_H
