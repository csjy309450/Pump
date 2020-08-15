/**
 * @file pump_core_handle.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of of handler interface.
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

#include "pump_core/pump_core_handle.h"

namespace Pump
{
namespace Core
{

CHandleBase::CHandleBase(PUMP_HANDLE_TYPE emType)
    : m_emType(emType)
    , m_handle(PUMP_NULL)
{
    
}

CHandleBase::CHandleBase(PUMP_HANDLE_TYPE emType, pump_handle_t handle)
    : m_emType(emType)
    , m_handle(handle)
{
}

CHandleBase::CHandleBase()
    : m_emType(PUMP_HANDLE_DEFAULT)
    , m_handle(PUMP_NULL)
{
}

CHandleBase::~CHandleBase()
{
}

pump_pvoid_t CHandleBase::GetHandle() const
{
    return m_handle;
}

PUMP_HANDLE_TYPE CHandleBase::GetHandleType() const
{
    return m_emType;
}

}
}
