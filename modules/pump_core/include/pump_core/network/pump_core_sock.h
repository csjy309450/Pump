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

#ifndef PUMP_CORE_SOCK_H
#define PUMP_CORE_SOCK_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_handle.h"
#include "pump_core/network/pump_core_addr.h"
#include "pump_core/pump_core_statemachine.h"
#include "pump_core/time/pump_core_time.h"

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

namespace Pump
{
namespace Core
{
namespace Net
{


class CAddr;
class CSockHandle;

class PUMP_CORE_CLASS CSockState
    : public ::Pump::Core::CStateMachineBase
{
public:
    typedef enum tagPUMP_SOCK_STATE
    {
        PUMP_SOCK_DEFAULT = 0,
        PUMP_SOCK_CREATED,      ///< sock initialized
        PUMP_SOCK_LISTENED,    ///< sock listened (waiting for connect<server>)
        PUMP_SOCK_CONNECTED,   ///< sock connected(async<client/server>)
        PUMP_SOCK_SHUTDOWNED,  ///< sock shutdown(async<client/server>)
        PUMP_SOCK_CLOSED           ///< close(async<client/server>)
    } PUMP_SOCK_STATE;
};

/*
 * @brief socket operations, all operations are synchronous.
 */
class PUMP_CORE_CLASS CSock
    : public ::Pump::Core::CHandleBase
{
public:
    friend class CSockHandle;
public:
    ~CSock();
    static CSock* CreateSock();
    static CSock* CreateSock(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto);
    static pump_int32_t DestroySock(CSock* pSock);

    pump_int32_t Open(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto);
    pump_int32_t Close(pump_bool_t bForce=PUMP_TRUE);
    pump_int32_t Shutdown(pump_int32_t iHow);
    pump_int32_t Bind(const CAddr & refAddr);
    pump_int32_t Listen(pump_int32_t iBackLog);
    pump_int32_t ListenExt(const CAddr & refAddr, pump_int32_t iBackLog);
    CSock* Accept();
    CSock* AcceptExt(pump_uint32_t dwTimeOut);
    pump_int32_t Connect(const CAddr & refAddr);
    pump_int32_t ConnectExt(const CAddr & refAddr, pump_uint32_t dwTimeOut);
    pump_int32_t Send(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize);
    pump_int32_t Sendn(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut);
    pump_int32_t SendExt(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout);
    pump_int32_t Recv(pump_pvoid_t pBuf, pump_uint32_t dwBufSize);
    pump_int32_t Recvn(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut);
    pump_int32_t RecvExt(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout);
    pump_int32_t SendTo(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, const CAddr & refLocalAddr);
    pump_int32_t RecvFrom(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, CAddr & refLocalAddr);

    pump_int32_t BroadCastOn();
    pump_int32_t JoinMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr);
    pump_int32_t LeaveMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr);
    pump_int32_t SetNonBlock(pump_bool_t bBlock);
    pump_int32_t GetTTL(pump_int32_t & iTTL);
    pump_int32_t SetTTL(pump_int32_t iTTL);
    pump_int32_t SetMultiCastTTL(pump_int32_t iTTL);
    pump_int32_t SetTOS(pump_int32_t iTOS);
    pump_int32_t GetTOS(pump_int32_t & iTOS);
    pump_int32_t SetReuseAddr(pump_bool_t bReuse);
    pump_int32_t SetTimeout(pump_int32_t iSendTimeout, pump_int32_t iRecvTimeout);
    pump_int32_t SetBuffSize(pump_int32_t iSendBuffSize, pump_int32_t iRecvBuffSize);
    pump_int32_t GetBuffSize(pump_int32_t & iSendBuffSize, pump_int32_t & iRecvBuffSize);
    pump_int32_t LingerOff();
    pump_int32_t LingerOn(pump_int32_t iTimeoutSec);
    pump_int32_t SetNoDelay(pump_bool_t bDelay);
    pump_int32_t Ioctl(pump_int32_t iCmd, ...); 

    pump_sock_t GetSock() const;
    const CAddr * GetAddr() const;
    CSockState::PUMP_SOCK_STATE GetSockState() const;
    virtual pump_bool_t Available() const;

private:
    CSock();
    CSock(pump_handle_t sock, const CAddr & refAddr);

private:
    //pump_sock_t m_handle;
    CAddr m_addr;
    CSockState m_objState;
};

class PUMP_CORE_CLASS CSockHandle
    : public ::Pump::Core::CHandleBase
{
public:
    static CSockHandle* CreateSock();
    static CSockHandle* CreateSock(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto);
    static pump_int32_t DestroySock(CSockHandle* pHSock);
    
    pump_int32_t Open(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto);
    pump_int32_t Close(pump_bool_t bForce = PUMP_TRUE);
    pump_int32_t Shutdown(pump_int32_t iHow);
    pump_int32_t Bind(const CAddr & refAddr);
    pump_int32_t Listen(pump_int32_t iBackLog);
    pump_int32_t ListenExt(const CAddr & refAddr, pump_int32_t iBackLog);
    CSockHandle* Accept();
    CSockHandle* AcceptExt(pump_uint32_t dwTimeOut);
    pump_int32_t Connect(const CAddr & refAddr);
    pump_int32_t ConnectExt(const CAddr & refAddr, pump_uint32_t dwTimeOut);
    pump_int32_t Send(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize);
    pump_int32_t Sendn(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut);
    pump_int32_t SendExt(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout);
    pump_int32_t Recv(pump_pvoid_t pBuf, pump_uint32_t dwBufSize);
    pump_int32_t Recvn(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut);
    pump_int32_t RecvExt(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout);
    pump_int32_t SendTo(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, const CAddr & refLocalAddr);
    pump_int32_t RecvFrom(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, CAddr & refLocalAddr);

    pump_int32_t BroadCastOn();
    pump_int32_t JoinMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr);
    pump_int32_t LeaveMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr);
    pump_int32_t SetNonBlock(pump_bool_t bBlock);
    pump_int32_t GetTTL(pump_int32_t & iTTL);
    pump_int32_t SetTTL(pump_int32_t iTTL);
    pump_int32_t SetMultiCastTTL(pump_int32_t iTTL);
    pump_int32_t SetTOS(pump_int32_t iTOS);
    pump_int32_t GetTOS(pump_int32_t & iTOS);
    pump_int32_t SetReuseAddr(pump_bool_t bReuse);
    pump_int32_t SetTimeout(pump_int32_t iSendTimeout, pump_int32_t iRecvTimeout);
    pump_int32_t SetBuffSize(pump_int32_t iSendBuffSize, pump_int32_t iRecvBuffSize);
    pump_int32_t GetBuffSize(pump_int32_t & iSendBuffSize, pump_int32_t & iRecvBuffSize);
    pump_int32_t LingerOff();
    pump_int32_t LingerOn(pump_int32_t iTimeoutSec);
    pump_int32_t SetNoDelay(pump_bool_t bDelay);
    pump_int32_t Ioctl(pump_int32_t iCmd, ...);

    pump_sock_t GetSock() const;
    const CAddr * GetAddr() const;
    CSockState::PUMP_SOCK_STATE GetSockState() const;
    virtual pump_bool_t Available() const;

private:
    virtual ~CSockHandle();
    CSockHandle();
    CSockHandle(CSock * pSock);
};

}
}
}

#endif // PUMP_CORE_SOCK_H