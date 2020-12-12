// dyanamic_load_dll.cpp : 定义控制台应用程序的入口点。
//

#include "pump_core/pump_core_string.h"
#include "pump_plugin/pump_plugin_config.h"
#define PUMP_PPLUG_BUILD_DLL 1
#include "pump_plugin/pump_pplug.h"
#include "pump_plugin/pump_plugin_log.h"

typedef struct tagPUMP_PLUGIN_REQUEST
{

} PUMP_PLUGIN_REQUEST, *LPPUMP_PLUGIN_REQUEST;

typedef struct tagPUMP_PLUGIN_RESPONSE
{

} PUMP_PLUGIN_RESPONSE, *LPPUMP_PLUGIN_RESPONSE;

typedef void(*PUMP_PSLOT_CB_Responce)(const PUMP_PLUGIN_RESPONSE*);

typedef struct tagPUMP_PLUGIN_ENTRY_API
{
    int(*fnPUMP_PLUGIN_Init)();
    int(*fnPUMP_PLUGIN_Cleanup)();
    int(*fnPUMP_PLUGIN_SetResponceCallback)(PUMP_PSLOT_CB_Responce);
    int(*fnPUMP_PLUGIN_Request)(const PUMP_PLUGIN_REQUEST *);
    int(*fnPUMP_PLUGIN_Start)();
} PUMP_PLUGIN_ENTRY_API, *LPPUMP_PLUGIN_ENTRY_API;

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Hello(LPPUMP_PLUGIN_ENTRY_API pCb);
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Init();
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Cleanup();
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_SetResponceCallback(PUMP_PSLOT_CB_Responce cb);
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Request(const PUMP_PLUGIN_REQUEST * req);
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Start();

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Hello(LPPUMP_PLUGIN_ENTRY_API pCb)
{
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Hello()");
    if (!pCb)
    {
        return -1;
    }
    pCb->fnPUMP_PLUGIN_Init = &PUMP_PLUGIN_Init;
    pCb->fnPUMP_PLUGIN_Cleanup = &PUMP_PLUGIN_Cleanup;
    pCb->fnPUMP_PLUGIN_SetResponceCallback = &PUMP_PLUGIN_SetResponceCallback;
    pCb->fnPUMP_PLUGIN_Request = &PUMP_PLUGIN_Request;
    pCb->fnPUMP_PLUGIN_Start = &PUMP_PLUGIN_Start;
    return 0;
}

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Init()
{
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Init()");
    return 0;
}

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Cleanup()
{
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Cleanup()");
    return 0;
}

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_SetResponceCallback(PUMP_PSLOT_CB_Responce cb)
{
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_SetResponceCallback()");
    return 0;
}

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Request(const PUMP_PLUGIN_REQUEST * req)
{
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Request()");
    return 0;
}

PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PLUGIN_Start()
{
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Start()");
    return 0;
}
