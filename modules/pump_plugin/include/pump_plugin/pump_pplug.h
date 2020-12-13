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
    char * req_msg;
    size_t req_size;
} PUMP_PPLUG_REQUEST, *LPPUMP_PPLUG_REQUEST;

typedef struct tagPUMP_PPLUG_RESPONSE
{

} PUMP_PPLUG_RESPONSE, *LPPUMP_PPLUG_RESPONSE;

typedef void(*PUMP_PPLUG_CB_Responce)(const PUMP_PPLUG_RESPONSE*);

typedef struct tagPUMP_PPLUG_ENTRY_API
{
    int(*pfnPUMP_PPLUG_Init)();
    int(*pfnPUMP_PPLUG_Cleanup)();
    int(*pfnPUMP_PPLUG_Start)();
    int(*pfnPUMP_PPLUG_SetResponceCallback)(PUMP_PPLUG_CB_Responce);
    int(*pfnPUMP_PPLUG_Request)(const PUMP_PPLUG_REQUEST *);
} PUMP_PPLUG_ENTRY_API, *LPPUMP_PPLUG_ENTRY_API;

typedef struct tagPUMP_PPLUG_INFO
{
    char szPlugName[64];
    char szPlugVersion[16];
} PUMP_PPLUG_INFO, *LPPUMP_PPLUG_INFO;

typedef struct tagPUMP_PPLUG_META
{
    PUMP_PPLUG_INFO struInfo;
    PUMP_PPLUG_ENTRY_API struEntryApi;
} PUMP_PPLUG_META, *LPPUMP_PPLUG_META;

typedef int(*PUMP_PPLUG_CB_Hello)(LPPUMP_PPLUG_META pMeta);

#define PUMP_PLUGIN_TOSTR_(str_) #str_
#define PUMP_PLUGIN_TOSTR(str_) PUMP_PLUGIN_TOSTR_(str_)
#define PUMP_PLUG_VERSION(major_, minor_,build_) PUMP_PLUG_VERSION_(major_, minor_,build_)
#define PUMP_PLUG_VERSION_(major_, minor_,build_) PUMP_PLUGIN_TOSTR(major_)"."PUMP_PLUGIN_TOSTR(minor_)"."PUMP_PLUGIN_TOSTR(build_)
#define PUMP_PPLUG_FNAME(pre_, indx_) PUMP_PPLUG_FNAME_(pre_, indx_)
#define PUMP_PPLUG_FNAME_(pre_, indx_) pre_##indx_

/**
 * plugin developer must cite in cpp file.
 */
#define PUMP_PPLUG_DEF(name_,major_, minor_,build_) \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Hello(LPPUMP_PPLUG_META pMeta); \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Init(); \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Cleanup(); \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Start(); \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_SetResponceCallback(PUMP_PPLUG_CB_Responce cb); \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Request(const PUMP_PPLUG_REQUEST * req); \
\
static int PUMP_PPLUG_FNAME(name_,Init)(); \
static int PUMP_PPLUG_FNAME(name_,Cleanup)(); \
static int PUMP_PPLUG_FNAME(name_,Start)(); \
static int PUMP_PPLUG_FNAME(name_,Request)(const PUMP_PPLUG_REQUEST * req); \
\
static PUMP_PPLUG_META s_pluginMeta = \
{ \
    {  \
        PUMP_PLUGIN_TOSTR(name_), \
        PUMP_PLUG_VERSION(major_, minor_,build_) \
    }, \
    { \
        &PUMP_PPLUG_Init, \
        &PUMP_PPLUG_Cleanup, \
        &PUMP_PPLUG_Start, \
        &PUMP_PPLUG_SetResponceCallback, \
        &PUMP_PPLUG_Request \
    } \
}; \
static const char* s_pluginName = s_pluginMeta.struInfo.szPlugName; \
static const char* s_pluginVersion = s_pluginMeta.struInfo.szPlugVersion; \
\
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Hello(LPPUMP_PPLUG_META pMeta) \
{ \
PUMP_PLUGIN_INFO("PUMP_PLUGIN_Hello() %s@%s",PUMP_PLUGIN_TOSTR(name_),PUMP_PLUG_VERSION(major_, minor_,build_)); \
    if (!pMeta) \
    { \
        return -1; \
    } \
    *pMeta = s_pluginMeta; \
    return 0; \
} \
\
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Init() \
{ \
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Init()"); \
    return PUMP_PPLUG_FNAME(name_,Init)(); \
} \
\
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Cleanup() \
{ \
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Cleanup()"); \
    return PUMP_PPLUG_FNAME(name_,Cleanup)(); \
} \
\
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_SetResponceCallback(PUMP_PPLUG_CB_Responce cb) \
{ \
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_SetResponceCallback()"); \
    return 0; \
} \
\
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Request(const PUMP_PPLUG_REQUEST * req) \
{ \
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Request()"); \
    return PUMP_PPLUG_FNAME(name_,Request)(req); \
} \
 \
PUMP_PPLUG_API int PUMP_CALLBACK PUMP_PPLUG_Start() \
{ \
    PUMP_PLUGIN_INFO("PUMP_PLUGIN_Start()"); \
    return PUMP_PPLUG_FNAME(name_,Start)(); \
}

#endif // PUMP_PPLUG_H
