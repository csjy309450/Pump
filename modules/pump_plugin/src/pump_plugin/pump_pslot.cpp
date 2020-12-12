#include "pump_macro/pump_pre.h"
#include <string>
#include <iostream>
#if (PUMP_PSLOT_LUA_MODE == 1)
#include "SLB3/namespace.h"
#include "SLB3/internal/class.h"
#include "SLB3/implementation.h"
#else
#include "pump_core/pump_core_dllso.h"
#endif // PUMP_PSLOT_LUA_MODE
#include "pump_core/pump_core_types.h"
#include "pump_core/file/pump_core_file.h"
#include "pump_core/pump_core_api.h"
#include "pump_plugin/pump_pslot.h"
#include "pump_plugin/pump_plugin_config.h"
#include "pump_plugin/pump_pslot_lua_context.h"
#include "pump_plugin/__pump_plugin_inner_interface.h"
#include "pump_plugin/__pump_plugin_global_ctrl.h"
#include "pump_plugin/pump_plugin_log.h"
#include "pump_plugin/pump_pplug.h"

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_Init()
{
    if (::Pump::Plugin::__CPumpPluginGlobalCtrl::IsInit())
    {
        return PUMP_OK;
    }
    if (::Pump::Plugin::__CPumpPluginGlobalCtrl::Create() != PUMP_OK)
    {
        return PUMP_ERROR;
    }
    if (::Pump::Plugin::__CPumpPluginGlobalCtrl::Init()!=PUMP_OK)
    {
        return PUMP_ERROR;
    }
#if (PUMP_PLUGIN_LUA_MODE == 1)
    SLB3::Register<::Pump::Core::Thread::CThread>(::Pump::Slot::GetGlobalLuaContext().L());
    SLB3::Register<::Pump::Core::Thread::CMutex>(::Pump::Slot::GetGlobalLuaContext().L());
    SLB3::Register<::Pump::Slot::CLuaThreadData>(::Pump::Slot::GetGlobalLuaContext().L());
    SLB3::Register<::Pump::Slot::CLuaThread>(::Pump::Slot::GetGlobalLuaContext().L());
#endif // PUMP_PLUGIN_LUA_MODE
    return PUMP_OK;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_Cleanup()
{
    if (!::Pump::Plugin::__CPumpPluginGlobalCtrl::IsInit())
    {
        return PUMP_OK;
    }
    if (::Pump::Plugin::__CPumpPluginGlobalCtrl::Cleanup() != PUMP_OK)
    {
        return PUMP_ERROR;
    }

    return ::Pump::Plugin::__CPumpPluginGlobalCtrl::Destroy();
}

PUMP_PLUGIN_API pump_int32_t PUMP_CALLBACK PUMP_PSLOT_InjectLocalLogger(pump_handle_t hLogger)
{
    if (!::Pump::Plugin::__CPumpPluginGlobalCtrl::IsInit())
    {
        // error: need init.
        return PUMP_ERROR;
    }
    return ::Pump::Plugin::__CPumpPluginGlobalCtrl::SetLogger(hLogger);
}

PUMP_PLUGIN_API  int PUMP_CALLBACK PUMP_PSLOT_Config(const PUMP_PSLOT_CONF * pConf)
{
    if (!::Pump::Plugin::__CPumpPluginGlobalCtrl::IsInit())
    {
        return PUMP_ERROR;
    }
    if (!pConf)
    {
        return PUMP_ERROR;
    }
    ::Pump::Plugin::__CPumpPluginGlobalCtrl::SetPslotConfig(*pConf);
    return PUMP_OK;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginInstall(const char* szPkgPath)
{
    return PUMP_OK;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginUninstall(const char* szName, const char* szVersion)
{
    return PUMP_OK;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_GetInstalledPluginsInfo(char * pBuf, pump_uint32_t nSize)
{
    return PUMP_OK;
}

PUMP_PLUGIN_API pump_plugin_id PUMP_CALLBACK PUMP_PSLOT_PluginLoad(const char* szName, const char* szVersion)
{
    PUMP_PLUGIN_INFO("PUMP_PSLOT_PluginLoad in");
    if (!::Pump::Plugin::__CPumpPluginGlobalCtrl::IsInit())
    {
        return -1;
    }
    std::string strPluginDir = ::Pump::Plugin::__CPumpPluginGlobalCtrl::GetPslotConfig().szInstallPath;
    strPluginDir += szName;
    strPluginDir += "@";
    strPluginDir += szVersion;
    // 1.检索插件安装路径
    pump_handle_t hDir = PUMP_CORE_DirOpen(strPluginDir.c_str());
    if (hDir == PUMP_INVALID_HANDLE)
    {
        PUMP_PLUGIN_ERROR("plugin not installed");
        return -1;
    }
    PUMP_CORE_DirClose(hDir);
    // 2.检索插件入口
#if (PUMP_PLUGIN_LUA_MODE == 1) // 使用lua模式
    std::string strEntry = strPluginDir + "/script/entry.lua";
#else
    std::string strEntry = strPluginDir + "/bin/entry.dll";
#endif // (PUMP_PLUGIN_LUA_MODE == 1)
    {
        PUMP_PLUGIN_ERROR("Invalid plug-in");
        return -1;
    }
#if (PUMP_PLUGIN_LUA_MODE == 1) // 使用lua模式
    ::Pump::Slot::CLSLoadEntry loadTestPlugin(szName, szVersion);
    ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptObject(loadTestPlugin);
    std::cout << loadTestPlugin.GetOut() << std::endl;
    try 
    {
        PUMP_PSLOT_INFO("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptString("SLB." + std::string(szName) + ".Check();").c_str());
    } catch (std::exception e) 
    {
        PUMP_PSLOT_ERROR("%s\n", e.what());
    }

    PUMP_PSLOT_INFO("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptString("SLB." + std::string(szName) + ".Init();").c_str());
    PUMP_PSLOT_INFO("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptString("SLB." + std::string(szName) + ".Load();").c_str());
    //printf("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaFunction("SLB." + std::string(szName) /*+ ".Post"*/, 4).c_str());
    //PUMP_CORE_Sleep(3000);
    PUMP_PSLOT_INFO("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptString("SLB." + std::string(szName) + ":Post(5);").c_str());
    const char* ss = lua_tostring(::Pump::Slot::GetGlobalLuaContext().L(), 1);
    //while (1)
    //{
    //    PUMP_CORE_Sleep(300);
    //    printf("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptString("SLB." + std::string(szName) + ":Post(6);").c_str());
    //    printf("%s\n", ::Pump::Slot::GetGlobalLuaContext().RunLuaScriptString("SLB." + std::string(szName) + ":Post(4);").c_str());
    //}
#else // c模式
    // 1. load plugin entry.dll
    pump_module_t hModule = PUMP_CORE_LoadDSo(strEntry.c_str());
    if (hModule==PUMP_INVALID_MODULE)
    {
        PUMP_PLUGIN_ERROR("Load plug-in entry.dll failed");
        return -1;
    }
    // 2. get and call plugin hello api
    PUMP_PPLUG_CB_Hello pfnPUMP_PSLOT_Hello = (PUMP_PPLUG_CB_Hello)PUMP_CORE_GetDsoSym(hModule, "PUMP_PSLOT_Hello");
    if (pfnPUMP_PSLOT_Hello==NULL)
    {
        PUMP_PLUGIN_ERROR("Get plug-in PUMP_PSLOT_Hello() failed");
        return -1;
    }
    // 3.call hello get plugin entry APIs.
    PUMP_PPLUG_ENTRY_API struPCb;
    memset(&struPCb, 0, sizeof(struPCb));
    pfnPUMP_PSLOT_Hello(&struPCb);
#endif // PUMP_PLUGIN_LUA_MODE
    return 0;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginUnload(pump_plugin_id iPID)
{
    return PUMP_OK;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_GetPluginInfo(pump_plugin_id iPID, char* pBuf, pump_uint32_t nSize)
{
    return PUMP_OK;
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginConfig(pump_plugin_id iPID, const char * szInfo)
{
    return PUMP_OK;
}

PUMP_PLUGIN_API void PUMP_CALLBACK PUMP_PSLOT_SetPluginResponceCallback(int iPID, PUMP_PSLOT_CB_PluginResponce cb)
{
}

PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginRequest(int iPID, const PUMP_PPLUG_REQUEST * pRequest)
{
    return PUMP_OK;
}

namespace Pump
{
namespace Slot
{

class CPluginClient;

PUMP_PLUGIN_CXXAPI CPluginClient  * PUMP_CALLBACK PUMP_PSLOT_INNER_GetPluginClientByIDLocked(pump_plugin_id pid)
{
    return NULL;
}


PUMP_PLUGIN_CXXAPI void PUMP_CALLBACK PUMP_PSLOT_INNER_PluginClientByIDUnLock(pump_plugin_id pid)
{
}

}
}