#ifndef PUMP_EVENT_CONFIG_H
#define PUMP_EVENT_CONFIG_H
#include "pump_macro/pump_pre.h"

#ifdef PUMP_EVENT_BUILD_DLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK __cdecl
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP __declspec(dllexport)
#define PUMP_EVENT_API extern "C" PUMP_EVENT_EXP
#define PUMP_EVENT_CLASS PUMP_EVENT_EXP
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP
#define PUMP_EVENT_API extern "C"
#define PUMP_EVENT_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#elif defined PUMP_EVENT_USING_DLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP __declspec(dllimport)
#define PUMP_EVENT_API extern "C" PUMP_EVENT_EXP
#define PUMP_EVENT_CLASS PUMP_EVENT_EXP
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP
#define PUMP_EVENT_API extern "C"
#define PUMP_EVENT_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#elif defined PUMP_EVENT_BUILD_SLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP
#define PUMP_EVENT_API extern "C"
#define PUMP_EVENT_CLASS
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP
#define PUMP_EVENT_API extern "C"
#define PUMP_EVENT_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#elif defined PUMP_EVENT_USING_SLL
#ifdef PUMP_OS_WINDOWS
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP
#define PUMP_EVENT_API extern "C"
#define PUMP_EVENT_CLASS
#elif defined PUMP_OS_POSIX
#ifndef PUMP_CALLBACK
#define PUMP_CALLBACK
#endif // PUMP_CALLBACK
#define PUMP_EVENT_EXP
#define PUMP_EVENT_API extern "C"
#define PUMP_EVENT_CLASS  
#else
#error OS not support!
#endif // PUMP_OS_WINDOWS
#else
#error must be one of {PUMP_EVENT_BUILD_DLL, PUMP_EVENT_USING_DLL, PUMP_EVENT_BUILD_SLL, PUMP_EVENT_USING_SLL}
#endif // PUMP_EVENT_BUILD_DLL

#ifdef PUMP_OS_WINDOWS
#define PUMP_INFINITE   INFINITE 
#elif (defined PUMP_OS_POSIX)
#define PUMP_INFINITE   0xffffffff 
#endif // PUMP_OS_WINDOWS

//#define PUMP_TEMP_EXP(class_,temp_arg_) template class PUMP_EVENT_CLASS class_ temp_arg_;

#endif // PUMP_EVENT_CONFIG_H
