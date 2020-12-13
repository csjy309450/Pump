#ifndef PUMP_PSLOT_PCLIENT_H
#define PUMP_PSLOT_PCLIENT_H
#include "pump_macro/pump_pre.h"
#include <string>
#include <map>
#include "pump_core/pump_core_dllso.h"
#include "pump_core/pump_core_object.h"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_plugin/pump_pplug.h"
#include "pump_plugin/pump_pslot.h"

namespace Pump
{
namespace Plugin
{

class CPluginClient
    : public ::Pump::Core::CObjectBase
    , public ::Pump::Core::CNonCopyable
{
public:
    CPluginClient(pump_module_t m_hModule, const PUMP_PPLUG_META & struPluginMeta);
    virtual ~CPluginClient();
    //const CJson & GetPluginInfo() const;
    //const CJson & GetPluginConfig() const;
    pump_int32_t Init();
    pump_int32_t Cleanup();
    pump_int32_t SetResponceCallback(PUMP_PPLUG_CB_Responce cb);
    pump_int32_t Request(const PUMP_PPLUG_REQUEST * req);
    pump_int32_t Start();

    pump_module_t GetModule() const;
private:
    CPluginClient();
private:
    pump_module_t m_hModule; // 一旦创建CPluginClient，m_hModule的所有者移交给CPluginClient
    const PUMP_PPLUG_META m_struPluginMeta;
    //CMutex m_csThis;
    //pump_plugin_id m_pid; ///< pid
    //CJson m_jsonPluginInfo; ///< 固定插件信息，包括插件名、版本号等
    //CJson m_jsonPluginConfig; ///< 插件的动态配置信息，应该定期读取一次文件
};

class CPluginClientMgr 
    : public ::Pump::Core::CObjectMgrBase
{
public:
    typedef std::map<std::string, CPluginClient*> PluginClientContainerType;
public:
    /**构造新的插件客户端对象，包括加载操作*/
    pump_plugin_id PluginLoad(const char* szName, const char* version);
    /**销毁插件客户端对象，包括卸载操作*/
    void PluginUnload(pump_plugin_id pid);
    /**获取插件配置信息*/
    std::string GetPluginInfo(pump_plugin_id pid);
    /*设置插件响应回调*/
    void SetPluginResponceCallback(pump_plugin_id pid, PUMP_PSLOT_CB_PluginResponce cb);
    /**下发请求*/
    int PluginRequest(pump_plugin_id iPID, const PUMP_PPLUG_REQUEST * pRequest);

private:
    PluginClientContainerType m_objPCContainer;
};


}
}

#endif //PUMP_PSLOT_PCLIENT_H


