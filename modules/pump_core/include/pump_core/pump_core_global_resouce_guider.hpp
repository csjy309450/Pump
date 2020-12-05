/**
 * @file pump_core_global_resouce_guider.hpp
 * @brief Library Header.
 * @details Declaration of global resource guider.
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

#ifndef PUMP_CORE_GLOBAL_RESOURCE_GUIDER_HPP
#define PUMP_CORE_GLOBAL_RESOURCE_GUIDER_HPP

#include "pump_core/pump_core_config.h"

namespace Pump
{
namespace Core
{

PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadLock();
PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadUnlock();

template <class T>
class CGlobalResouceGuider
{
public:
    explicit CGlobalResouceGuider(T * ptr)
        : m_ptr(ptr)
    {
        PUMP_CORE_INNER_GlobalCtrlReadLock();
    }
    virtual ~CGlobalResouceGuider()
    {
        m_ptr = (NULL);
        PUMP_CORE_INNER_GlobalCtrlReadUnlock();
    }
    CGlobalResouceGuider(const CGlobalResouceGuider & other)
        : m_ptr(other.m_ptr)
    {
        PUMP_CORE_INNER_GlobalCtrlReadLock();
    }
    T * GetPtr()
    {
        return m_ptr;
    }
private:
    CGlobalResouceGuider();
private:
    T * m_ptr;
};

}
}

#endif // PUMP_CORE_GLOBAL_RESOURCE_GUIDER_HPP

