/**
 * @file pump_core_dllso.h
 * @brief Library Header.
 * @details Declaration of pump_core dll dynamic load interface.
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

#ifndef PUMP_CORE_DLLSO_H
#define PUMP_CORE_DLLSO_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/os_wrapper/pump_core_os_dllso.h"
#include "pump_core/pump_core_nonop.h"

namespace Pump
{
namespace Core
{

class CDllso
    : public ::Pump::Core::CNonCopyOperator
{
public:
    explicit CDllso(const char* szDllPath);
    virtual ~CDllso();
    pump_bool_t IsLoaded() const;
    pump_int32_t Load();
    pump_int32_t Unload();
    pump_proc_t GetSymbol(const char* szProcName);
private:
    CDllso();
private:
    pump_module_t m_hModule;
    char m_szDllPath[PUMP_MAX_DSO_PATH];
};

}
}

#endif // PUMP_CORE_DLLSO_H

