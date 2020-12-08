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
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_noncopyable.h"

#define PUMP_INVALID_MODULE NULL
#define PUMP_MAX_DSO_PATH (260) //windows has 260

typedef pump_handle_t pump_module_t;
typedef void* pump_proc_t;

/**
 * PUMP_LoadDSo load dll so or other library.
 * @param pPath (IN) dll name hpr.dll or libhpr.so
 * @return PUMP_INVALID_HANDEL fail, else succ
 * @sa PUMP_UnloadDSo(), PUMP_GetDsoSym()
 */
PUMP_CORE_API pump_module_t PUMP_CALLBACK PUMP_CORE_LoadDSo(const char* pPath);

/**use for linux param*/
PUMP_CORE_API pump_module_t PUMP_CALLBACK PUMP_CORE_LoadDSoEx(const char* pPath, pump_int32_t iMode);

/**
 * PUMP_LoadDSo unload dll so or other library.
 * @param hModule (IN) success ruturned by PUMP_LoadDSo.
 * @return 0 success, -1 fail.
 * @sa PUMP_CORE_LoadDSo(), PUMP_CORE_GetDsoSym()
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_UnloadDSo(pump_module_t hModule);

/**
 * PUMP_LoadDSo get the symbol of the dll/so by name.
 * @param hModule (IN) success ruturned by PUMP_LoadDSo.
 * @param pName (IN) symbol name.
 * @return PUMP_INVALID_HANDEL fail, else succ
 * @sa PUMP_CORE_UnloadDSo
 */
PUMP_CORE_API pump_proc_t PUMP_CALLBACK PUMP_CORE_GetDsoSym(pump_module_t hModule, const char* pName);

namespace Pump
{
namespace Core
{

class CDllso
    : public ::Pump::Core::CNonCopyable
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

