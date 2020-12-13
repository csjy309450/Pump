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
    pump_module_t m_hModule; // һ������CPluginClient��m_hModule���������ƽ���CPluginClient
    const PUMP_PPLUG_META m_struPluginMeta;
    //CMutex m_csThis;
    //pump_plugin_id m_pid; ///< pid
    //CJson m_jsonPluginInfo; ///< �̶������Ϣ��������������汾�ŵ�
    //CJson m_jsonPluginConfig; ///< ����Ķ�̬������Ϣ��Ӧ�ö��ڶ�ȡһ���ļ�
};

class CPluginClientMgr 
    : public ::Pump::Core::CObjectMgrBase
{
public:
    typedef std::map<std::string, CPluginClient*> PluginClientContainerType;
public:
    /**�����µĲ���ͻ��˶��󣬰������ز���*/
    pump_plugin_id PluginLoad(const char* szName, const char* version);
    /**���ٲ���ͻ��˶��󣬰���ж�ز���*/
    void PluginUnload(pump_plugin_id pid);
    /**��ȡ���������Ϣ*/
    std::string GetPluginInfo(pump_plugin_id pid);
    /*���ò����Ӧ�ص�*/
    void SetPluginResponceCallback(pump_plugin_id pid, PUMP_PSLOT_CB_PluginResponce cb);
    /**�·�����*/
    int PluginRequest(pump_plugin_id iPID, const PUMP_PPLUG_REQUEST * pRequest);

private:
    PluginClientContainerType m_objPCContainer;
};


}
}

#endif //PUMP_PSLOT_PCLIENT_H


