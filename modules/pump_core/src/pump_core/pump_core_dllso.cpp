#ifdef PUMP_OS_POSIX
#include <dlfcn.h>
#endif // PUMP_OS_POSIX
#include "pump_core/pump_core_dllso.h"

PUMP_CORE_API pump_module_t PUMP_CALLBACK PUMP_CORE_LoadDSo(const char* pPath)
{
#ifdef PUMP_OS_POSIX
    return PUMP_CORE_LoadDSoEx(pPath, RTLD_NOW | RTLD_GLOBAL);
#elif (defined PUMP_OS_WINDOWS)
    return PUMP_CORE_LoadDSoEx(pPath, 0);
#endif // PUMP_OS_POSIX
}

PUMP_CORE_API pump_module_t PUMP_CALLBACK PUMP_CORE_LoadDSoEx(const char* pPath, pump_int32_t iMode)
{
#ifdef PUMP_OS_POSIX
    if (!pPath)
    {
        return PUMP_INVALID_MODULE;
    }
    return dlopen(pPath, iMode);
#elif (defined PUMP_OS_WINDOWS)
    HMODULE h = NULL;

    char szPath[MAX_PATH] = { 0 };
    char * p = NULL;

    if (!pPath)
    {
        return PUMP_INVALID_MODULE;
    }

    //replace '/' by '\\' if exist '/'.
#   if (PUMP_MSC_VER >= 1500 && !defined PUMP_OS_WINCE)
    strcpy_s(szPath, sizeof(szPath), pPath);
#   else
    strncpy(szPath, pPath, sizeof(szPath));
#   endif

    p = szPath;
    while ((p = strchr(p, '/')) != NULL)
    {
        *p = '\\';
    }
#   if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wPath[260] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, szPath, strlen(szPath), wPath, sizeof(wPath));
#       if defined PUMP_OS_WINCE
    h = LoadLibraryEx(wPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#       else
    h = LoadPackagedLibrary(wPath, 0);
#       endif
#   else
    h = LoadLibraryEx(szPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#   endif
    if (h == NULL)
    {
#   if defined PUMP_OS_WINCE
        h = LoadLibraryEx(wPath, NULL, 0);
#   elif defined PUMP_OS_WINPHONE
        return PUMP_INVALID_MODULE;
#   else
        h = LoadLibraryEx(szPath, NULL, 0);
#   endif
    }
    return h;
#endif // PUMP_OS_POSIX
}

/**
* Function:	PUMP_UnloadDSo
* Desc:		unload dll
* Input:		@param hModule: dll handle, reutrned by PUMP_LoadDSo.
* Output:
* Return:		if success return handle of dll, otherwise reutrn NULL.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_UnloadDSo(pump_handle_t hModule)
{
#ifdef PUMP_OS_POSIX
    if ((hModule) && (!dlclose(hModule)))
    {
        return PUMP_OK;
    }

    return PUMP_ERROR;
#elif (defined PUMP_OS_WINDOWS)
    if ((hModule != NULL) && (FreeLibrary((HMODULE)hModule)))
    {
        return PUMP_OK;
    }

    return PUMP_ERROR;
#endif // PUMP_OS_POSIX
}

/**
* Function:	PUMP_GetDsoSym
* Desc:		get proc address by name
* Input:		@param hModule: dll handle, returned by PUMP_LoadDSo.
@param pName: proc name.
* Output:
* Return:		if success return handle of dll, otherwise reutrn NULL.
* */
PUMP_CORE_API pump_proc_t PUMP_CALLBACK PUMP_CORE_GetDsoSym(pump_handle_t hModule, const char* pName)
{
#ifdef PUMP_OS_POSIX
    if (hModule && pName)
    {
        return (pump_proc_t)dlsym(hModule, pName);
    }

    return NULL;
#elif (defined PUMP_OS_WINDOWS)
    if (hModule == NULL || !pName)
    {
        return NULL;
    }

#if defined PUMP_OS_WINCE
    return (pump_proc_t)GetProcAddressA((HMODULE)hModule, pName);
#else
    return (pump_proc_t)GetProcAddress((HMODULE)hModule, pName);
#endif
#endif // PUMP_OS_POSIX
}

namespace Pump
{
namespace Core
{

CDllso::CDllso()
    : m_hModule(PUMP_INVALID_MODULE)
{
    memset(m_szDllPath, 0, sizeof(m_szDllPath));
}

CDllso::CDllso(const char* szDllPath)
    : m_hModule(PUMP_INVALID_MODULE)
{
    m_hModule = NULL;
    memset(m_szDllPath, 0, sizeof(m_szDllPath));
    if ((szDllPath) && (strlen(szDllPath) < PUMP_MAX_DSO_PATH))
    {
        strncpy(m_szDllPath, szDllPath, PUMP_MAX_DSO_PATH);
    }
}

CDllso::~CDllso()
{
    try
    {
        if (m_hModule)
        {
            Unload();
        }
        m_hModule = PUMP_INVALID_MODULE;
    }
    catch (...)
    {

    }
}

pump_bool_t CDllso::IsLoaded() const
{
    return (m_hModule != PUMP_INVALID_MODULE);
}

pump_int32_t CDllso::Load()
{
    if (m_szDllPath[0] == 0)
    {
        return PUMP_ERROR;
    }
    if (m_szDllPath)
    {
        return PUMP_OK;
    }
    pump_module_t hModular = PUMP_CORE_LoadDSo(m_szDllPath);
    if (hModular == PUMP_INVALID_MODULE)
    {
        return PUMP_ERROR;
    }
    m_hModule = hModular;
    return PUMP_OK;
}

pump_int32_t CDllso::Unload()
{
    if (m_szDllPath[0] == 0 || m_hModule == PUMP_INVALID_MODULE)
    {
        return PUMP_ERROR;
    }
    return PUMP_CORE_UnloadDSo(m_hModule);;
}

pump_proc_t CDllso::GetSymbol(const char* szProcName)
{
    if (m_szDllPath[0] == 0 || m_hModule == PUMP_INVALID_MODULE)
    {
        return PUMP_NULL;
    }
    return PUMP_CORE_GetDsoSym(m_hModule, szProcName);
}

}
}

