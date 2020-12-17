/**
 * @file pump_core_addr.cpp
 * @brief Library Implementation.
 * @details Implementation of network address interface.
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

#include "pump_core/pump_core_addr.h"

namespace Pump
{
namespace Core
{
namespace Net
{

CAddr::CAddr() 
    : m_iAf(-1)
    , m_sPort(-1)
{
    memset(&m_unIP, 0, sizeof(m_unIP));
}

CAddr::CAddr(pump_int32_t iAf, const char* szAddr, pump_uint16_t sPort)
    : m_iAf(-1)
    , m_sPort(-1)
{
    memset(&m_unIP, 0, sizeof(m_unIP));
    size_t dwSize = strlen(szAddr);
    if (iAf == PUMP_AF_INET && dwSize <= 16)
    {
        strcpy(m_unIP.ipv4_, szAddr);
        m_iAf = (iAf);
        m_sPort = (sPort);
    }
#if (defined PUMP_OS_SUPPORT_IPV6)
    else if (iAf == PUMP_AF_INET6 && dwSize > 16 && dwSize <= 46)
    {
        strcpy(m_unIP.ipv6_, szAddr);
        m_iAf = (iAf);
        m_sPort = (sPort);
    }
#endif // (defined PUMP_OS_SUPPORT_IPV6)
}

CAddr::CAddr(pump_int32_t iAf, const std::string & strAddr, pump_uint16_t sPort)
    : m_iAf(-1)
    , m_sPort(-1)
{
    memset(&m_unIP, 0, sizeof(m_unIP));
    size_t dwSize = strAddr.size();
    if (iAf == PUMP_AF_INET && dwSize <= 16)
    {
        if (!dwSize)
        {
            strcpy(m_unIP.ipv4_, strAddr.c_str());
        }
        m_iAf = (iAf);
        m_sPort = (sPort);
    }
#if (defined PUMP_OS_SUPPORT_IPV6)
    else if (iAf == PUMP_AF_INET6 && dwSize <= 46)
    {
        if (!dwSize)
        {
            strcpy(m_unIP.ipv6_, strAddr.c_str());
        }
        m_iAf = (iAf);
        m_sPort = (sPort);
    }
#endif // (defined PUMP_OS_SUPPORT_IPV6)
}

CAddr::CAddr(const CAddr & other)
    : m_iAf(other.m_iAf)
    , m_sPort(other.m_sPort)
    , m_unIP(other.m_unIP)
{
    
}

CAddr::~CAddr()
{
    
}

pump_bool_t CAddr::Available() const
{
    if (m_iAf<0
        || (m_sPort<0))
    {
        return PUMP_FALSE;
    }
    return PUMP_TRUE;
}

CAddr & CAddr::operator=(const CAddr & other)
{
    m_iAf = (other.m_iAf);
    m_sPort = (other.m_sPort);
    m_unIP = (other.m_unIP);
    return *this;
}

pump_int32_t CAddr::SetAddr(const PUMP_ADDR & refAddr)
{
    return PUMP_OK;
}

pump_int32_t CAddr::GetAddr(PUMP_ADDR & refAddr) const
{
    if (!this->Available())
    {
        return PUMP_ERROR;
    }
    memset(&refAddr, 0, sizeof(refAddr));
    if (m_iAf == PUMP_AF_INET)
    {
        if (m_unIP.ipv4_[0]==0)
        {
            refAddr.sa_.sin4_.sin_addr.s_addr = PUMP_INADDR_ANY;
            refAddr.sa_.sin4_.sin_family = PUMP_AF_INET;
            refAddr.sa_.sin4_.sin_port = ::htons(m_sPort);
        }
        else
        {
            refAddr.sa_.sin4_.sin_addr.s_addr = ::inet_addr(m_unIP.ipv4_);
            if (refAddr.sa_.sin4_.sin_addr.s_addr == INADDR_NONE)
            {
                return PUMP_ERROR;
            }
            refAddr.sa_.sin4_.sin_family = PUMP_AF_INET;
            refAddr.sa_.sin4_.sin_port = ::htons(m_sPort);
        }
    }
#if (defined PUMP_OS_SUPPORT_IPV6)
    else if (m_iAf == PUMP_AF_INET6)
    {
        if (m_unIP.ipv6_[0] == 0)
        {
            memset(&refAddr.sa_.sin6_.sin6_addr,0, 16);
            refAddr.sa_.sin6_.sin6_family = PUMP_AF_INET6;
            refAddr.sa_.sin6_.sin6_port = ::htons(m_sPort);
        }
        else
        {
            /* TODO 20200224 lack of IPv6 method (str -> int) */
            refAddr.sa_.sin6_.sin6_family = PUMP_AF_INET6;
            refAddr.sa_.sin6_.sin6_port = ::htons(m_sPort);
        }
    }
#endif // (defined PUMP_OS_SUPPORT_IPV6)
    else
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CAddr::SetAddrExp(const PUMP_ADDR_EXP & refAddr)
{
    return PUMP_OK;
}

pump_int32_t CAddr::GetAddrExp(PUMP_ADDR_EXP & refAddr) const
{
    return PUMP_OK;
}

pump_int32_t CAddr::GetPort(pump_uint16_t & refPort) const
{
    return PUMP_OK;
}

pump_int32_t CAddr::SetPort(pump_uint16_t sPort)
{
    return PUMP_OK;
}

pump_int32_t CAddr::SetAddrFamily(pump_int32_t iAf)
{
    return PUMP_OK;
}

pump_int32_t CAddr::GetAddrFamily(pump_int32_t & refAf) const
{
    return PUMP_OK;
}

pump_int32_t CAddr::GetAddrStr(std::string & refAddr) const
{
    if (!this->Available())
    {
        return PUMP_ERROR;
    }

    if (m_iAf == PUMP_AF_INET)
    {
        refAddr = m_unIP.ipv4_;
    }
#if (defined PUMP_OS_SUPPORT_IPV6)
    else if (m_iAf == PUMP_AF_INET6)
    {
        refAddr = m_unIP.ipv6_;
    }
#endif // (defined PUMP_OS_SUPPORT_IPV6)
    else
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}
pump_int32_t CAddr::SetAddrStr(const std::string & refAddr)
{
    if (m_iAf == PUMP_AF_INET)
    {
        if (refAddr.size()==0 || refAddr.size() > PUMP_MAX_IPV4_ADDRLEN)
        {
            return PUMP_ERROR;
        }
        memset(&m_unIP, 0, sizeof(m_unIP));
        strncpy(m_unIP.ipv4_, refAddr.c_str(), refAddr.size());
    }
#if (defined PUMP_OS_SUPPORT_IPV6)
    else if (m_iAf == PUMP_AF_INET6)
    {
        if (refAddr.size() == 0 || refAddr.size() > PUMP_MAX_IPV6_ADDRLEN)
        {
            return PUMP_ERROR;
        }
        memset(&m_unIP, 0, sizeof(m_unIP));
        strncpy(m_unIP.ipv6_, refAddr.c_str(), refAddr.size());
    }
#endif // (defined PUMP_OS_SUPPORT_IPV6)
    else
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

}
}
}