/**
 * @file pump_core_noncopyable.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of noncopyable object interface.
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

#include "pump_core/pump_core_nonop.h"

namespace Pump
{
namespace Core
{

CNonCopyOperator::CNonCopyOperator() {}

CNonCopyOperator::~CNonCopyOperator()
{}

CNonNewOperator::CNonNewOperator()
{}

CNonNewOperator::~CNonNewOperator()
{}

}
}