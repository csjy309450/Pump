/**
 * @file pump_core_environment.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of environment variable control interface.
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

#include "pump_core/pump_core_environment.h"
#include <stdlib.h>
#ifdef PUMP_OS_POSIX
#include <unistd.h>
extern char ** environ;
#endif // PUMP_OS_POSIX

namespace Pump
{
namespace Core
{

CEnvironment::CEnvironment()
#ifdef PUMP_OS_WINDOWS
    : m_pEnvStrings(PUMP_NULL)
    , m_pEnvStringsTmp(PUMP_NULL)
#elif (defined PUMP_OS_POSIX)
    : m_pEnvStrings(PUMP_NULL)
    , m_pEnvStringsTmp(PUMP_NULL)
#endif // PUMP_OS_WINDOWS
{}

CEnvironment::~CEnvironment()
{
    FreeEnvStrings();
}

pump_bool_t CEnvironment::GetEnvStrings()
{
#ifdef PUMP_OS_WINDOWS
    m_pEnvStrings = ::GetEnvironmentStringsA();
    if (m_pEnvStrings == PUMP_NULL)
    {
        return FALSE;
    }
#elif (defined PUMP_OS_POSIX)
    m_pEnvStrings = environ;
#endif // PUMP_OS_WINDOWS
    return PUMP_TRUE;
}

pump_void_t CEnvironment::FreeEnvStrings()
{
#ifdef PUMP_OS_WINDOWS
    if (m_pEnvStrings != PUMP_NULL)
    {
        ::FreeEnvironmentStringsA((pump_char_t *)m_pEnvStrings);
        m_pEnvStrings = PUMP_NULL;
    }
#elif (defined PUMP_OS_POSIX)
    m_pEnvStrings = PUMP_NULL;
#endif // PUMP_OS_WINDOWS
}

pump_bool_t CEnvironment::SetEnvVariable(const pump_char_t * pName, const pump_char_t * pValue)
{
    if (pName == PUMP_NULL || pValue == PUMP_NULL)
    {
        return PUMP_FALSE;
    }
#ifdef PUMP_OS_WINDOWS
    return ::SetEnvironmentVariableA(pName, pValue);
#elif (defined PUMP_OS_POSIX)
    std::string strEnvVal = pName;
    strEnvVal +="=";
    strEnvVal += +pValue;
    return (::putenv(strEnvVal.c_str()) == 0?PUMP_TRUE:PUMP_FALSE);
#endif // PUMP_OS_WINDOWS
}

pump_bool_t CEnvironment::GetEnvVariable(const pump_char_t * pName, pump_char_t * pValue, pump_uint32_t nLen)
{
    if (pName == PUMP_NULL || pValue == PUMP_NULL || nLen == 0)
    {
        return PUMP_FALSE;
    }
#ifdef PUMP_OS_WINDOWS
    DWORD nRet = ::GetEnvironmentVariableA(pName, pValue, nLen);
    if (nRet <= 0 || nRet > nLen)
    {
        return PUMP_FALSE;
    }
#elif (defined PUMP_OS_POSIX)
    char *pEnv = getenv(pName);
    if (pEnv || ::strlen(pEnv)>nLen)
    {
        return PUMP_FALSE;
    }
    ::strncpy(pValue, pEnv, nLen);
#endif // PUMP_OS_WINDOWS
    return PUMP_TRUE;
}

const pump_char_t * CEnvironment::GetFirstString()
{
#ifdef PUMP_OS_WINDOWS
    m_pEnvStringsTmp = (pump_char_t *)m_pEnvStrings;
    if (m_pEnvStringsTmp[0] == 0)
    {
        return PUMP_NULL;
    }
    return m_pEnvStringsTmp;
#elif (defined PUMP_OS_POSIX)
    m_pEnvStringsTmp = m_pEnvStrings;
    if (m_pEnvStringsTmp[0] == 0)
    {
        return PUMP_NULL;
    }
    return m_pEnvStringsTmp[0];
#endif // PUMP_OS_WINDOWS
}

const pump_char_t * CEnvironment::GetNextString()
{
#ifdef PUMP_OS_WINDOWS
    m_pEnvStringsTmp = m_pEnvStringsTmp + strlen(m_pEnvStringsTmp) + 1;
    if (m_pEnvStringsTmp[0] == 0)
    {
        return PUMP_NULL;
    }
    return m_pEnvStringsTmp;
#elif (defined PUMP_OS_POSIX)
    if (*m_pEnvStringsTmp == PUMP_NULL)
    {
        return PUMP_NULL;
    }
    m_pEnvStringsTmp++;
    return *m_pEnvStringsTmp;
#endif // PUMP_OS_WINDOWS
}

}
}

