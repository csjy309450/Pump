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
#if (defined _WIN32)
#   include <WinSock2.h>
#   include <ws2ipdef.h>
#   include <ws2tcpip.h>
#   pragma warning(disable: 4102)
#   pragma comment(lib,"ws2_32.lib") 
#elif (defined PUMP_OS_POSIX)
#   include <sys/socket.h>
#   include <sys/ioctl.h>
#   include <arpa/inet.h>
#   include <netinet/in.h>
#   include <net/if.h>
#endif // (defined _WIN32)
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"

#if (defined PUMP_OS_WINDOWS)
#   if (PUMP_MSC_VER < 1310)
#       ifndef s6_addr
struct in6_addr
{
    union
    {
        pump_uint8_t Byte[16];
        pump_uint16_t Word[8];
    } u;
};
#           define in_addr6 in6_addr
/*
 * Defines to matching RFC 2553.
 */
#           define _S6_un   u
#           define _S6_u8   Byte
#           define s6_addr _S6_un._S6_u8
/*
 * Defines for us.
 */
#           define s6_bytes u.Byte
#           define s6_words u.Word
#       endif // s6_addr
typedef struct
{
    pump_int16_t sin6_family; /* PUMP_AF_INT6 */
    pump_uint16_t sin6_port; /* Transport level port Num. */
    pump_ulong_t sin6_flowinfo; /* IPv6 flow infomation */
    struct in6_addr sin6_addr; /* IPv6 address */
    pump_ulong_t sin6_scope_id; /* set of interfaces for a scope */
} pump_sockaddr_in6;
typedef sockaddr_in pump_sockaddr_in;
#   else
typedef sockaddr_in pump_sockaddr_in;
typedef sockaddr_in6 pump_sockaddr_in6;
#   endif // (PUMP_MSC_VER < 1310)
#elif (defined PUMP_OS_POSIX)
typedef sockaddr_in pump_sockaddr_in;
typedef sockaddr_in6 pump_sockaddr_in6;
#else
#   error OS Not Support!
#endif // (defined PUMP_OS_WINDOWS)

#if (defined PUMP_OS_WINDOWS)
typedef SOCKET pump_sock_t;
#define PUMP_INVALID_SOCKET ((pump_handle_t)INVALID_SOCKET)
#elif (defined PUMP_OS_POSIX)
typedef pump_fd_t pump_sock_t;
#define PUMP_INVALID_SOCKET ((pump_handle_t)(-1))
#endif // (defined PUMP_OS_WINDOWS)

typedef struct tagPUMP_ADDR
{
    union
    {
        pump_sockaddr_in sin4_; /* IPv4 addr */
#if (defined PUMP_OS_SUPPORT_IPV6)
        pump_sockaddr_in6 sin6_; /* IPv6 addr */
#endif // (defined PUMP_OS_SUPPORT_IPV6)
    } sa_;
} PUMP_ADDR;

typedef struct tagPUMP_ADDR_EXP
{
    pump_int32_t af_; /* PUMP_AF_INET or PUMP_AF_INET6 */
    union
    {
        pump_uint32_t _addr4_; /* IPv4 (integer) addr, net-order */
        pump_uint8_t addr6_[16]; /* IPv6 (integer) addr, net-order */
    } addr_;
    pump_uint16_t port_; /* port Num., host-order */
    pump_uint16_t reserved_; /* reserved */
} PUMP_ADDR_EXP;

#if (defined PUMP_OS_WINDOWS)
typedef struct fd_set PUMP_FDSET, *LPPUMP_FDSET;
#   if(_WIN32_WINNT >= 0x0600)
typedef struct pollfd PUMP_POLLFD, *LPPUMP_POLLFD;
#   else
typedef struct tagPUMP_POLLFD {
    pump_sock_t  fd;
    pump_int16_t   events;
    pump_int16_t   revents;
} PUMP_POLLFD, *LPPUMP_POLLFD;
#   endif // (_WIN32_WINNT >= 0x0600)
#elif (defined PUMP_OS_POSIX)
typedef fd_set PUMP_FDSET, *LPPUMP_FDSET;
typedef struct pollfd PUMP_POLLFD, *LPPUMP_POLLFD;
#else
#endif // (defined PUMP_OS_WINDOWS)

#define PUMP_INADDR_ANY INADDR_ANY
#define PUMP_AF_INET AF_INET
#if (defined PUMP_OS_SUPPORT_IPV6)
#define PUMP_AF_INET6 AF_INET6
#endif // (defined PUMP_OS_SUPPORT_IPV6)

#define PUMP_MAX_IPV4_ADDRLEN 16
#define PUMP_MAX_IPV6_ADDRLEN 46
#define PUMP_MAX_IP_ADDRLEN PUMP_MAX_IPV6_ADDRLEN

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