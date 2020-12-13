#ifndef PUMP_PLUGIN_CONFIG_H
#define PUMP_PLUGIN_CONFIG_H
#include "pump_macro/pump_pre.h"

#ifdef PUMP_COMPILER_CXX
#   define PUMP_EXTERNC_FLAG extern "C"
#else
#   define PUMP_EXTERNC_FLAG 
#endif // PUMP_COMPILER_CXX
#ifdef PUMP_PLUGIN_BUILD_DLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK __cdecl
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP __declspec(dllexport)
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS PUMP_PLUGIN_EXP
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS  
#   else
#       error OS not support!
#   endif // PUMP_OS_WINDOWS
#elif defined PUMP_PLUGIN_USING_DLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP __declspec(dllimport)
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS PUMP_PLUGIN_EXP
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS  
#   else
#       error OS not support!
#   endif // PUMP_OS_WINDOWS
#elif defined PUMP_PLUGIN_BUILD_SLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS  
#   else
#       error OS not support!
#   endif // PUMP_OS_WINDOWS
#elif defined PUMP_PLUGIN_USING_SLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_API PUMP_EXTERNC_FLAG
#           define PUMP_PLUGIN_CXXAPI PUMP_PLUGIN_EXP
#           define PUMP_PLUGIN_CLASS  
#   else
#       error OS not support!
#   endif // PUMP_OS_WINDOWS
#else
#   error must be one of {PUMP_PLUGIN_BUILD_DLL, PUMP_PLUGIN_USING_DLL, PUMP_PLUGIN_BUILD_SLL, PUMP_PLUGIN_USING_SLL}
#endif // PUMP_PLUGIN_BUILD_DLL

//#define PUMP_TEMP_EXP(class_,temp_arg_) template class PUMP_PLUGIN_CLASS class_ temp_arg_;

#endif // PUMP_PLUGIN_CONFIG_H
