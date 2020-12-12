#ifndef PUMP_PPLUG_H
#define PUMP_PPLUG_H

#ifdef PUMP_COMPILER_CXX
#   define PUMP_EXTERNC_FLAG extern "C"
#else
#   define PUMP_EXTERNC_FLAG 
#endif // PUMP_COMPILER_CXX
#ifdef PUMP_PPLUG_BUILD_DLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK __cdecl
#       endif // PUMP_CALLBACK
#           define PUMP_PPLUG_EXP __declspec(dllexport)
#           define PUMP_PPLUG_API PUMP_EXTERNC_FLAG PUMP_PPLUG_EXP
#           define PUMP_PPLUG_CXXAPI PUMP_PPLUG_EXP
#           define PUMP_PPLUG_CLASS PUMP_PPLUG_EXP
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PPLUG_EXP
#           define PUMP_PPLUG_API PUMP_EXTERNC_FLAG
#           define PUMP_PPLUG_CXXAPI PUMP_PPLUG_EXP
#           define PUMP_PPLUG_CLASS  
#   else
#       error OS not support!
#   endif // PUMP_OS_WINDOWS
#endif // PUMP_PPLUG_BUILD_DLL

typedef struct tagPUMP_PPLUG_REQUEST
{

} PUMP_PPLUG_REQUEST, *LPPUMP_PPLUG_REQUEST;

typedef struct tagPUMP_PPLUG_RESPONSE
{

} PUMP_PPLUG_RESPONSE, *LPPUMP_PPLUG_RESPONSE;

typedef void(*PUMP_PPLUG_CB_Responce)(const PUMP_PPLUG_RESPONSE*);

typedef struct tagPUMP_PPLUG_ENTRY_API
{
    int(*pfnPUMP_PPLUG_Init)();
    int(*pfnPUMP_PPLUG_Cleanup)();
    int(*pfnPUMP_PPLUG_SetResponceCallback)(PUMP_PPLUG_CB_Responce);
    int(*pfnPUMP_PPLUG_Request)(const PUMP_PPLUG_REQUEST *);
    int(*pfnPUMP_PPLUG_Start)();
} PUMP_PPLUG_ENTRY_API, *LPPUMP_PPLUG_ENTRY_API;

typedef int(*PUMP_PPLUG_CB_Hello)(LPPUMP_PPLUG_ENTRY_API pCb);

#endif // PUMP_PPLUG_H
