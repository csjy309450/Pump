#ifndef PUMP_PLUGIN_H
#define PUMP_PLUGIN_H

typedef struct tagPUMP_PLUGIN_REQUEST
{

} PUMP_PLUGIN_REQUEST, *LPPUMP_PLUGIN_REQUEST;

typedef struct tagPUMP_PLUGIN_RESPONSE
{

} PUMP_PLUGIN_RESPONSE, *LPPUMP_PLUGIN_RESPONSE;

typedef void(*PUMP_PLUGIN_CB_Responce)(const PUMP_PLUGIN_RESPONSE*);

typedef struct tagPUMP_PLUGIN_ENTRY_API
{
    int(*pfnPUMP_PLUGIN_Init)();
    int(*pfnPUMP_PLUGIN_Cleanup)();
    int(*pfnPUMP_PLUGIN_SetResponceCallback)(PUMP_PLUGIN_CB_Responce);
    int(*pfnPUMP_PLUGIN_Request)(const PUMP_PLUGIN_REQUEST *);
    int(*pfnPUMP_PLUGIN_Start)();
} PUMP_PLUGIN_ENTRY_API, *LPPUMP_PLUGIN_ENTRY_API;

typedef int(*PUMP_PLUGIN_CB_Hello)(LPPUMP_PLUGIN_ENTRY_API pCb);

#endif // PUMP_PLUGIN_H
