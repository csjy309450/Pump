/**
 * @file pump_core_addr.h
 * @brief Library Header.
 * @details Declaration of network address interface.
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

#ifndef PUMP_CORE_ADDR_H
#define PUMP_CORE_ADDR_H
#include "pump_macro/pump_pre.h"
#include <string>
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/os_wrapper/pump_core_os_addr.h"

namespace Pump
{
namespace Core
{
namespace Net
{

class PUMP_CORE_CLASS CAddr
{
public:
    typedef union
    {
        pump_char_t ipv4_[PUMP_MAX_IPV4_ADDRLEN];
        pump_char_t ipv6_[PUMP_MAX_IPV6_ADDRLEN];
    } IPType;
public:
    CAddr();
    CAddr(pump_int32_t iAf, const char* szAddr, pump_uint16_t sPort);
    CAddr(pump_int32_t iAf, const std::string & strAddr, pump_uint16_t sPort);
    CAddr(const CAddr & other);
    ~CAddr();
    CAddr & operator=(const CAddr & other);

    pump_bool_t Available() const;
    pump_int32_t SetAddr(const PUMP_ADDR & refAddr);
    pump_int32_t GetAddr(PUMP_ADDR & refAddr) const;
    pump_int32_t SetAddrExp(const PUMP_ADDR_EXP & refAddr);
    pump_int32_t GetAddrExp(PUMP_ADDR_EXP & refAddr) const;
    pump_int32_t GetPort(pump_uint16_t & refPort) const;
    pump_int32_t SetPort(pump_uint16_t sPort);
    pump_int32_t SetAddrFamily(pump_int32_t iAf);
    pump_int32_t GetAddrFamily(pump_int32_t & refAf) const;
    pump_int32_t GetAddrStr(std::string & refAddr) const; // 20200416 [FIXME] Mustn't using std::string reference in export class or api, may cause crash.
    pump_int32_t SetAddrStr(const std::string & refAddr);

private:
    pump_int32_t m_iAf;
    IPType m_unIP;
    pump_int16_t m_sPort;
};

}
}
}

#endif // PUMP_CORE_ADDR_H