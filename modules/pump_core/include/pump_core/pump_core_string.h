/**
 * @file pump_core_string.h
 * @brief Library Header.
 * @details Declaration of string interface.
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

#ifndef PUMP_CORE_STRING_H
#define PUMP_CORE_STRING_H
#include "pump_macro/pump_pre.h"
#include <stdarg.h>
#ifdef PUMP_COMPILER_CXX
#include <sstream>
#include <string>
#endif // PUMP_COMPILER_CXX
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"

PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_Printf(const char* szFormate, ...);
#ifdef PUMP_COMPILER_CXX
/**
* @fn  template<typename T> bool PUMP_CORE_Str2Other(std::string & str,T & t)
* @brief string -> other type
* @return true or false
*/
template<typename T>
pump_bool_t PUMP_CORE_Str2Other(std::string & str, T & t)
{
    std::stringstream ss;
    ss << str;
    ss >> t;
    if ((ss.rdstate() & std::stringstream::badbit) == 1 ||
        (ss.rdstate() & std::stringstream::failbit) == 1){ // ss.fail() == true
        return PUMP_FALSE;
    }
    return PUMP_TRUE;
}

/** @fn  template<typename T> std::string other2str(T & t)
* @brief other type -> string
* @return outout string
*/
template<typename T>
std::string PUMP_CORE_Other2Str(T & t)
{
    std::stringstream ss;
    std::string str;
    ss << t;
    ss >> str;
    if ((ss.rdstate() & std::stringstream::badbit) == 1 ||
        (ss.rdstate() & std::stringstream::failbit) == 1){ // ss.fail() == true
        return "";
    }
    return str;
}

PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_StringTrim(const std::string & strSrc);
PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_StringReplase(const std::string &strSrc, const std::string &pattern, const std::string &target);
PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_Sprintf(const pump_str_t szFormate, ...);
PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_Vprintf(const pump_str_t szFormate, va_list argv);

#endif // PUMP_COMPILER_CXX
#endif // PUMP_CORE_STRING_H
