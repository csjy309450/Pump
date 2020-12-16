/**
 * @file pump_core_handle.h
 * @brief Library Header.
 * @details Declaration of handler interface.
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

#ifndef PUMP_CORE_HANDLE_H
#define PUMP_CORE_HANDLE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_statemachine.h"

namespace Pump
{
namespace Core
{

typedef enum tagPUMP_HANDLE_TYPE
{
    PUMP_HANDLE_DEFAULT = 0,
    PUMP_HANDLE_SOCKET,
    PUMP_HANDLE_FILE,
    PUMP_HANDLE_TIMER,
    PUMP_HANDLE_PIPE,
} PUMP_HANDLE_TYPE;

class PUMP_CORE_CLASS CHandleBase
{
public:
    CHandleBase(PUMP_HANDLE_TYPE emType);
    CHandleBase(PUMP_HANDLE_TYPE emType, pump_handle_t handle);
    virtual ~CHandleBase();
    virtual pump_bool_t Available() const = 0;
    pump_pvoid_t GetHandle() const;
    PUMP_HANDLE_TYPE GetHandleType() const;
protected:
    pump_pvoid_t m_handle;

private:
    CHandleBase();
    CHandleBase(const CHandleBase&);
    const PUMP_HANDLE_TYPE m_emType;
};

}
}

#endif // PUMP_CORE_HANDLE_H