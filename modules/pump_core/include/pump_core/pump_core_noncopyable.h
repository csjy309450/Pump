/**
 * @file pump_core_noncopyable.h
 * @brief Library Header.
 * @details Declaration of noncopyable object interface.
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

#ifndef PUMP_CROE_NONCOPYABLE_H
#define PUMP_CROE_NONCOPYABLE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"

namespace Pump
{
namespace Core
{

class PUMP_CORE_CLASS CNonCopyable
{
public:
  CNonCopyable();
  virtual ~CNonCopyable();
private:
  CNonCopyable(const CNonCopyable&);
  CNonCopyable(const CNonCopyable*);
  CNonCopyable & operator=(const CNonCopyable&);
};

}
}

#endif //PUMP_CROE_NONCOPYABLE_H
