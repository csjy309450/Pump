/**
 * @file pump_core_sock.h
 * @brief Library Inner Header.
 * @details Declaration of socket interface.
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

#ifndef PUMP_CORE_OS_SOCK_H
#define PUMP_CORE_OS_SOCK_H
#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_config.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/os_wrapper/pump_core_os_addr.h"
#include "pump_core/os_wrapper/pump_core_os_time.h"

#ifdef PUMP_OS_WINDOWS
#   define PUMP_SHUT_RW SD_BOTH
#   define PUMP_SHUT_RD SD_RECEIVE
#   define PUMP_SHUT_WR SD_SEND
#elif (defined PUMP_OS_POSIX)
#   define PUMP_SHUT_RW SHUT_REWD
#   define PUMP_SHUT_RD SHUT_RD
#   define PUMP_SHUT_WR SHUT_WR
#else 
#   error os not support.
#endif

/**init network module*/ 
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_InitNetwork();
/**cleanup network module*/ 
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_CleanupNetwork();

PUMP_CORE_API pump_sock_t PUMP_CALLBACK PUMP_CORE_SockCreate(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockClose(pump_sock_t hSock, pump_bool_t bForce = PUMP_TRUE);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockShutdown(pump_sock_t hSock, pump_int32_t iHow);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockBind(pump_sock_t hSock, const PUMP_ADDR * pAddr);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockListen(pump_sock_t hSock, pump_int32_t iBackLog);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockListenExt(pump_sock_t hSock, const PUMP_ADDR * pAddr, pump_int32_t iBackLog);
PUMP_CORE_API pump_sock_t PUMP_CALLBACK PUMP_CORE_SockLAccept(pump_sock_t hSock, PUMP_ADDR * pAddr = NULL, pump_int32_t nTimeOut = PUMP_INFINITE);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockConnect(pump_sock_t hSock, const PUMP_ADDR * pAddr);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockConnectExt(pump_sock_t hSock, const PUMP_ADDR * pAddr, pump_int32_t nTimeOut = PUMP_INFINITE);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSend(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSendn(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSendExt(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecv(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecvn(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecvExt(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSendTo(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize,const PUMP_ADDR * pLocalAddr);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecvFrom(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize, PUMP_ADDR * pLocalAddr);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockBroadCastOn(pump_sock_t hSock);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockJoinMultiCastGroup(pump_sock_t hSock, const PUMP_ADDR * pLocalAddr, const PUMP_ADDR * pMCGroupAddr);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockLeaveMultiCastGroup(pump_sock_t hSock, const PUMP_ADDR * pLocalAddr, const PUMP_ADDR * pMCGroupAddr);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetNonBlock(pump_sock_t hSock, pump_bool_t bBlock);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetTTL(pump_sock_t hSock, pump_int32_t * iTTL);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetTTL_V6(pump_sock_t hSock, pump_int32_t* iTTL);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTTL(pump_sock_t hSock, pump_int32_t iTTL);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTTL_V6(pump_sock_t hSock, pump_int32_t iTTL);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetMultiCastTTL(pump_sock_t hSock, pump_int32_t iTTL);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetMultiCastTTL_V6(pump_sock_t hSock, pump_int32_t iTTL);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTOS(pump_sock_t hSock, pump_int32_t iTOS);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetTOS(pump_sock_t hSock, pump_int32_t * iTOS);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetReuseAddr(pump_sock_t hSock, pump_bool_t bReuse);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTimeout(pump_sock_t hSock, pump_int32_t iSendTimeout, pump_int32_t iRecvTimeout);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetBuffSize(pump_sock_t hSock, pump_int32_t iSendBuffSize, pump_int32_t iRecvBuffSize);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetBuffSize(pump_sock_t hSock, pump_int32_t * iSendBuffSize, pump_int32_t * iRecvBuffSize);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockLingerOff(pump_sock_t hSock);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockLingerOn(pump_sock_t hSock, pump_int32_t iTimeoutSec);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetNoDelay(pump_sock_t hSock, pump_bool_t bDelay);
//PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Ioctl(pump_sock_t hSock, pump_int32_t iCmd, ...);

#endif // PUMP_CORE_OS_SOCK_H