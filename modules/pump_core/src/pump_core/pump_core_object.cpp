/**
 * @file pump_core_object.cpp
 * @brief Library Implementation.
 * @details Implementation of object system interface.
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

#include "pump_core/pump_core_object.h"
#include <cstddef>

namespace Pump
{
namespace Core
{

CObjectBase::CObjectBase(__CPrimitiveBase * pPrimitive)
    : m_pPrimitive(pPrimitive) {}

CObjectBase::CObjectBase()
    : m_pPrimitive(NULL)
{

}

CObjectBase::~CObjectBase()
{
    delete m_pPrimitive;
}

}
}