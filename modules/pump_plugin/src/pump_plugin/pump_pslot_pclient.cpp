#include "pump_plugin/pump_pslot_pclient.h"

namespace Pump
{
namespace Plugin
{

CPluginClient::CPluginClient(pump_module_t m_hModule, const PUMP_PPLUG_META & struPluginMeta)
    : m_hModule(m_hModule)
    , m_struPluginMeta(struPluginMeta)
{

}

CPluginClient::~CPluginClient()
{
    PUMP_CORE_UnloadDSo(m_hModule);
    m_hModule = PUMP_INVALID_MODULE;
}

pump_int32_t CPluginClient::Init()
{
    if (!m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Init)
    {
        return -1;
    }
    return m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Init();
}

pump_int32_t CPluginClient::Cleanup()
{
    if (!m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Cleanup)
    {
        return -1;
    }
    return m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Cleanup();
}

pump_int32_t CPluginClient::SetResponceCallback(PUMP_PPLUG_CB_Responce cb)
{
    if (!m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_SetResponceCallback)
    {
        return -1;
    }
    return m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_SetResponceCallback(cb);
}

pump_int32_t CPluginClient::Request(const PUMP_PPLUG_REQUEST * req)
{
    if (!m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Request)
    {
        return -1;
    }
    return m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Request(req);
}

pump_int32_t CPluginClient::Start()
{
    if (!m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Start)
    {
        return -1;
    }
    return m_struPluginMeta.struEntryApi.pfnPUMP_PPLUG_Start();
}

pump_module_t CPluginClient::GetModule() const
{
    return m_hModule;
}

}
}