#ifndef PUMP_HTTP_CONFIG_H
#define PUMP_HTTP_CONFIG_H

#include "pump_macro/pump_pre.h"

#ifdef PUMP_HTTP_BUILD_DLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK __cdecl
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP __declspec(dllexport)
#define PUMP_HTTP_API extern "C" PUMP_HTTP_EXP
#define PUMP_HTTP_CLASS PUMP_HTTP_EXP
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP
#define PUMP_HTTP_API extern "C"
#define PUMP_HTTP_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#elif defined PUMP_HTTP_USING_DLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP __declspec(dllimport)
#define PUMP_HTTP_API extern "C" PUMP_HTTP_EXP
#define PUMP_HTTP_CLASS PUMP_HTTP_EXP
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP
#define PUMP_HTTP_API extern "C"
#define PUMP_HTTP_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#elif defined PUMP_HTTP_BUILD_SLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP
#define PUMP_HTTP_API extern "C"
#define PUMP_HTTP_CLASS
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP
#define PUMP_HTTP_API extern "C"
#define PUMP_HTTP_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#elif defined PUMP_HTTP_USING_SLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP
#define PUMP_HTTP_API extern "C"
#define PUMP_HTTP_CLASS
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_HTTP_EXP
#define PUMP_HTTP_API extern "C"
#define PUMP_HTTP_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#else
#error must be one of {PUMP_HTTP_BUILD_DLL, PUMP_HTTP_USING_DLL, PUMP_HTTP_BUILD_SLL, PUMP_HTTP_USING_SLL}
#endif // PUMP_HTTP_BUILD_DLL

//#define PUMP_TEMP_EXP(class_,temp_arg_) template class PUMP_HTTP_CLASS class_ temp_arg_;

// pump_http version info
#ifndef PUMP_HTTP_VERSION_MAJOR
#   define PUMP_HTTP_VERSION_MAJOR 0
#endif // !PUMP_HTTP_VERSION_MAJOR
#ifndef PUMP_HTTP_VERSION_MINOR
#   define PUMP_HTTP_VERSION_MINOR 0
#endif // !PUMP_HTTP_VERSION_MINOR
#ifndef PUMP_HTTP_VERSION_PATCH
#   define PUMP_HTTP_VERSION_PATCH 9
#endif // !PUMP_HTTP_VERSION_PATCH
#ifndef PUMP_HTTP_VERSION_BUILD
#   define PUMP_HTTP_VERSION_BUILD 0
#endif // !PUMP_HTTP_VERSION_BUILD
#define __PUMP_HTTP_VERSION_STR(str_) #str_
#define __PUMP_HTTP_VERJOIN(major_,minor_,patch_,build_) __PUMP_HTTP_VERSION_STR(major_.minor_.patch_.build_)
#define PUMP_HTTP_VERSION __PUMP_HTTP_VERJOIN(PUMP_HTTP_VERSION_MAJOR,PUMP_HTTP_VERSION_MINOR,PUMP_HTTP_VERSION_PATCH,PUMP_HTTP_VERSION_BUILD)

#endif PUMP_HTTP_CONFIG_H