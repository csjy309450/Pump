/**
 * @file pump_core_config.h
 * @brief Library Header.
 * @details Declaration of pump_core global config macro.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#ifndef PUMP_CORE_CONFIG_H
#define PUMP_CORE_CONFIG_H
#include "pump_macro/pump_pre.h"

#ifdef PUMP_COMPILER_CXX
#   define PUMP_EXTERNC_FLAG extern "C"
#else
#   define PUMP_EXTERNC_FLAG 
#endif // PUMP_COMPILER_CXX
#ifdef PUMP_CORE_BUILD_DLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK __cdecl
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP __declspec(dllexport)
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG PUMP_CORE_EXP
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS PUMP_CORE_EXP
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS  
#   else
#       error os not supported.
#   endif // PUMP_OS_WINDOWS
#elif defined PUMP_CORE_USING_DLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP __declspec(dllimport)
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG PUMP_CORE_EXP
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS PUMP_CORE_EXP
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS  
#   else
#       error os not supported.
#   endif // PUMP_OS_WINDOWS
#elif defined PUMP_CORE_BUILD_SLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS  
#   else
#       error os not supported.
#   endif // PUMP_OS_WINDOWS
#elif defined PUMP_CORE_USING_SLL
#   ifdef PUMP_OS_WINDOWS
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS
#   elif defined PUMP_OS_POSIX
#       ifndef PUMP_CALLBACK
#           define PUMP_CALLBACK
#       endif // PUMP_CALLBACK
#           define PUMP_CORE_EXP
#           define PUMP_CORE_API PUMP_EXTERNC_FLAG
#           define PUMP_CORE_CXXAPI PUMP_CORE_EXP
#           define PUMP_CORE_CLASS  
#   else
#       error os not supported.
#   endif // PUMP_OS_WINDOWS
#else
#   error must be one of { PUMP_CORE_BUILD_DLL, PUMP_CORE_USING_DLL, PUMP_CORE_BUILD_SLL, PUMP_CORE_USING_SLL }
#endif // PUMP_CORE_BUILD_DLL

//#define PUMP_TEMP_EXP(class_,temp_arg_) template class PUMP_CORE_CLASS class_ temp_arg_;

#endif // PUMP_CORE_CONFIG_H
