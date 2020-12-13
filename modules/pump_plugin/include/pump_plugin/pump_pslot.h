#ifndef PUMP_PSLOT_H
#define PUMP_PSLOT_H

#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_types.h"
#include "pump_plugin/pump_plugin_config.h"
#include "pump_plugin/pump_pplug.h"

enum PUMP_PSLOT_LOG_LEVEL 
{
    
};

typedef pump_int32_t pump_plugin_id;

/**请求异步回调原型*/
typedef void(*PUMP_PSLOT_CB_PluginResponce)(pump_plugin_id, const PUMP_PPLUG_RESPONSE*);


typedef struct tagPUMP_PSLOT_CONF
{
    char szInstallPath[260];
} PUMP_PSLOT_CONF, *LPPUMP_PSLOT_CONF;

/**初始化接口*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_Init();

/**反初始化接口*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_Cleanup();

PUMP_PLUGIN_API pump_int32_t PUMP_CALLBACK PUMP_PSLOT_InjectLocalLogger(pump_handle_t hLogger);

/**
* 全局配置接口，实现日志、插件搜索路径等配置
* @param pConf slot配置结构体
*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_Config(const PUMP_PSLOT_CONF * pConf);

/**
* 插件安装接口，展开插件安装包，部署到指定路径
* @param szPkgPath 安装包路径
*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginInstall(const char* szPkgPath);

/**删除插件*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginUninstall(const char* szName, const char* szVersion = NULL);

/**
* 获取已安装插件信息，检索安装插件的路径，返回json格式的安装插件信息
* @param pBuf 保存信息的缓冲区，由调用者提供，主要是避免产生静态条件
* @param nSize 缓冲区长度
* @return 0(函数执行成功)，-1(调用失败，根据错误码缓冲区不够，重新调用)
*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_GetInstalledPluginsInfo(char * pBuf, pump_uint32_t nSize);

/**
* 加载插件接口
* @param szName 插件名
* @param szVersion 插件版本
* @return -1(失败)，>=0(插件资源ID)，用于卸载插件
*/
PUMP_PLUGIN_API pump_plugin_id PUMP_CALLBACK PUMP_PSLOT_PluginLoad(const char* szName, const char* szVersion = NULL);

/**
* 卸载插件接口
* @param iPID 插件资源ID
*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginUnload(pump_plugin_id iPID);

/**
* 获取已加载插件信息
* @param iPID 插件资源号
* @param szBuf 保存信息的缓冲区，由调用者提供，主要是避免产生静态条件
* @param nSize 缓冲区长度
* @return 0(函数执行成功)，-1(调用失败，根据错误码缓冲区不够，重新调用)
*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_GetPluginInfo(pump_plugin_id iPID, char* pBuf, pump_uint32_t nSize);

/**配置已加载插件*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginConfig(pump_plugin_id iPID, const char * szInfo);

/**异步回调设置*/
PUMP_PLUGIN_API void PUMP_CALLBACK PUMP_PSLOT_SetPluginResponceCallback(pump_plugin_id iPID, PUMP_PSLOT_CB_PluginResponce cb);

/**
* 向插件下发请求（异步）
* @param pRequest 请求报文
* @param nSize 数据长度
* @param cb 响应回调
*/
PUMP_PLUGIN_API int PUMP_CALLBACK PUMP_PSLOT_PluginRequest(pump_plugin_id iPID, const PUMP_PPLUG_REQUEST * pRequest);

#endif // PUMP_PSLOT_H