/**
 * @file pump_core_sock.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of socket interface.
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

#include "pump_core/network/pump_core_sock.h"
#ifdef PUMP_OS_WINDOWS

#elif (defined PUMP_OS_POSIX)
#include <netinet/tcp.h>
#include <fcntl.h>
#endif // PUMP_OS_WINDOWS
#include "pump_core/pump_core_api.h"
#include "pump_core/network/pump_core_multiplex.h"
#include "pump_core/time/pump_core_time.h"

#if defined(PUMP_OS_WINDOWS)
#define PUMP_SOCKET_ERROR() WSAGetLastError()
#define PUMP_SET_SOCKET_ERROR(errcode)	\
    do { WSASetLastError(errcode); } while (0)
#elif defined(PUMP_OS_POSIX)
#include <netinet/tcp.h>
#include <signal.h>
#include <fcntl.h>
#define closesocket close
#ifndef SOCKADDR
typedef sockaddr SOCKADDR;
typedef sockaddr* PSOCKADDR;
#endif
#define PUMP_SOCKET_ERROR() (errno)
#define PUMP_SET_SOCKET_ERROR(errcode) \
    do { errno = (errcode); } while (0)
#endif

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_InitNetwork()
{
#ifdef PUMP_OS_WINDOWS
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (::WSAStartup(sockVersion, &wsaData) != 0)
    {
        return PUMP_ERROR;
    }
#endif // PUMP_OS_WINDOWS
    return PUMP_OK;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_CleanupNetwork()
{
#ifdef PUMP_OS_WINDOWS
    if (::WSACleanup() != 0)
    {
        return PUMP_ERROR;
    }
#endif // PUMP_OS_WINDOWS
    return PUMP_OK;
}

PUMP_CORE_API pump_sock_t PUMP_CALLBACK PUMP_CORE_SockCreate(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto)
{
    return ::socket(iAf, iType, iProto);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockClose(pump_sock_t hSock, pump_bool_t bForce)
{
    if (bForce)
    {
        PUMP_CORE_SockShutdown(hSock, PUMP_SHUT_RW);
    }

    return ::closesocket(hSock);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockShutdown(pump_sock_t hSock, pump_int32_t iHow)
{
    return ::shutdown(hSock, iHow);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockBind(pump_sock_t hSock, const PUMP_ADDR * pAddr)
{
    if (!pAddr)
    {
        return PUMP_ERROR;
    }
    sockaddr_in sin4_ = pAddr->sa_.sin4_;
    if (sin4_.sin_family == AF_INET)
    {
        return ::bind(hSock, (PSOCKADDR)&pAddr->sa_, sizeof(sockaddr_in));
    }
    else
    {
        return ::bind(hSock, (PSOCKADDR)&pAddr->sa_, sizeof(PUMP_ADDR));
    }
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockListen(pump_sock_t hSock, pump_int32_t iBackLog)
{
    return ::listen(hSock, iBackLog);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockListenExt(pump_sock_t hSock, const PUMP_ADDR * pAddr, pump_int32_t iBackLog)
{
    if (hSock == PUMP_INVALID_SOCKET)
    {
        return PUMP_ERROR;
    }
    if (PUMP_CORE_SockBind(hSock, pAddr) == PUMP_ERROR)
    {
        return PUMP_ERROR;
    }
    return ::listen((pump_sock_t)hSock, iBackLog);
}

PUMP_CORE_API pump_sock_t PUMP_CALLBACK PUMP_CORE_SockLAccept(pump_sock_t hSock, PUMP_ADDR * pAddr, pump_int32_t dwTimeOut)
{
    pump_sock_t iTmpSockFd = (pump_sock_t)PUMP_INVALID_SOCKET;
    int iAddrSize = sizeof(pAddr->sa_);

    if (dwTimeOut == PUMP_INFINITE)
    {
        if (pAddr != NULL)
        {
            iTmpSockFd = ::accept(hSock, (sockaddr*)&pAddr->sa_, (socklen_t*)&iAddrSize);
        }
        else
        {
            iTmpSockFd = ::accept(hSock, NULL, NULL);
        }
    }
    else
    {
#if defined (PUMP_OS_WINDOWS)
        fd_set fdset_r;
        timeval tv = { dwTimeOut / 1000, (dwTimeOut % 1000) * 1000 };
        int iSelectRet = -1;

        FD_ZERO(&fdset_r);
        FD_SET(hSock, &fdset_r);

        iSelectRet = PUMP_CORE_Select((int)hSock + 1, &fdset_r, NULL, NULL, &tv);
        if (iSelectRet > 0 && FD_ISSET(hSock, &fdset_r))
#elif defined (PUMP_OS_POSIX)
        struct pollfd fds[1] = { 0 };
        int iPollRet = -1;
        fds[0].fd = hSock;
        fds[0].events = POLLRDNORM;
        iPollRet = PUMP_CORE_Poll(fds, 1, (pump_int32_t*)&dwTimeOut);
        if ((iPollRet > 0) && (fds[0].revents & POLLRDNORM))
#else
#error OS Not Implement.
#endif
        {
            if (pAddr != NULL)
            {
                iTmpSockFd = ::accept(hSock, (sockaddr*)&pAddr->sa_, (socklen_t*)&iAddrSize);
            }
            else
            {
                iTmpSockFd = ::accept(hSock, NULL, NULL);
            }
        }
    }

    if (iTmpSockFd != (pump_sock_t)PUMP_INVALID_SOCKET)
    {
        return iTmpSockFd;
    }

    return iTmpSockFd;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockConnect(pump_sock_t hSock, const PUMP_ADDR * pAddr)
{
    if (hSock == PUMP_INVALID_SOCKET)
    {
        return PUMP_ERROR;
    }
    return ::connect((pump_sock_t)hSock, (PSOCKADDR)&pAddr->sa_, sizeof(pump_sockaddr_in));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockConnectExt(pump_sock_t hSock, const PUMP_ADDR * pAddr, pump_int32_t dwTimeOut)
{
    if (!pAddr)
    {
        return PUMP_ERROR;
    }

    if (dwTimeOut == PUMP_INFINITE)
    {
        return ::connect(hSock, (PSOCKADDR)&pAddr->sa_, sizeof(sockaddr_in));
    }

    int iSystemLastError = 0;///[ADD] bug fixed if error, last errno is overwritten.
    pump_int32_t iSelectRet = -1;
    pump_int32_t iRet = PUMP_ERROR;

    PUMP_CORE_SockSetNonBlock(hSock, PUMP_TRUE);
    sockaddr_in sin4_ = pAddr->sa_.sin4_;
    if (sin4_.sin_family == AF_INET)
    {
        iRet = ::connect(hSock, (PSOCKADDR)&pAddr->sa_.sin4_, sizeof(pAddr->sa_.sin4_));
    }
    else
    {
        iRet = ::connect(hSock, (PSOCKADDR)&pAddr->sa_.sin6_, sizeof(pAddr->sa_.sin6_));
    }

    if (iRet == 0)//if success, return succ immediately.
    {
        return iRet;
    }

#if defined (OS_WINDOWS)
    if (HPR_SOCKET_ERROR() != WSAEWOULDBLOCK)
#else
    if (PUMP_SOCKET_ERROR() != EINPROGRESS)
#endif
    {
        return iRet;
    }

#if defined PUMP_OS_WINDOWS
    timeval tv;
    tv.tv_sec = dwTimeOut / 1000;
    tv.tv_usec = (dwTimeOut % 1000) * 1000;
    fd_set fdset_w;
    FD_ZERO(&fdset_w);
    FD_SET(hSock, &fdset_w);
    iSelectRet = PUMP_CORE_Select((int)hSock + 1, NULL, &fdset_w, NULL, &tv);
    if (iSelectRet > 0 && FD_ISSET(hSock, &fdset_w))
#elif defined PUMP_OS_POSIX
    struct pollfd fds[1] = { 0 };
    fds[0].fd = hSock;
    fds[0].events = POLLWRNORM;
    iSelectRet = PUMP_CORE_Poll(fds, 1, (pump_int32_t*)&dwTimeOut);
    if ((iSelectRet > 0) && (fds[0].revents & POLLWRNORM))
#endif
    {
        pump_int32_t iError;
        pump_int32_t iErrorLen;
        iErrorLen = sizeof(iError);
        pump_int32_t iTmp = ::getsockopt(hSock, SOL_SOCKET, SO_ERROR, (char*)&iError, (socklen_t*)&iErrorLen);
        if ((iTmp == 0) && (iError == 0))
        {
            iRet = 0;
        }
        else
        {
            if (iTmp == 0)
            {
                iSystemLastError = iError;
            }
            else
            {
                iSystemLastError = PUMP_SOCKET_ERROR();
            }
        }
    }
    else if (iSelectRet < 0)
    {
        iSystemLastError = PUMP_SOCKET_ERROR();
#if defined PUMP_OS_POSIX
        if (dwTimeOut == 0)
        {
            iSystemLastError = ETIMEDOUT;
        }
#endif
    }
    else  // iSelectRet == 0
    {
#if defined (PUMP_OS_WINDOWS)
        iSystemLastError = WSAETIMEDOUT;
#else
        iSystemLastError = ETIMEDOUT;
#endif
    }

    //bNonBlock = HPR_FALSE;
    ::PUMP_CORE_SockSetNonBlock(hSock, PUMP_FALSE);

    if (iRet != 0)
    {
        PUMP_SET_SOCKET_ERROR(iSystemLastError);
    }
    //////////////////////////////////////////////////////////////////////////

    return iRet;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSend(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize)
{
    return ::send(hSock, (char*)pBuf, dwBufSize, 0);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSendn(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut)
{
    int iSendLen = 0;
    int iTmpLen = 0;
    int iRet = -1;

#if defined (PUMP_OS_WINDOWS)
    timeval tv;
    fd_set fdset_w;

    if (dwTimeOut != PUMP_INFINITE)
    {
        tv.tv_sec = dwTimeOut / 1000;
        tv.tv_usec = (dwTimeOut % 1000) * 1000;
    }
    else
    {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    }
#endif

    do
    {
#if defined (PUMP_OS_WINDOWS)
        FD_ZERO(&fdset_w);
        FD_SET(hSock, &fdset_w);

        if (dwTimeOut == PUMP_INFINITE)
        {
            iRet = PUMP_CORE_Select((int)hSock + 1, NULL, &fdset_w, NULL, NULL);
        }
        else
        {
            iRet = PUMP_CORE_Select((int)hSock + 1, NULL, &fdset_w, NULL, &tv);
        }
        dwTimeOut = tv.tv_sec * 1000 + tv.tv_usec / 1000;

        if (iRet > 0 && FD_ISSET(hSock, &fdset_w))
#elif defined (PUMP_OS_POSIX)
        struct pollfd fds[1] = { 0 };
        fds[0].fd = hSock;
        fds[0].events = POLLWRNORM;
        iRet = PUMP_CORE_Poll(fds, 1, (pump_int32_t*)&dwTimeOut);
        if ((iRet > 0) && (fds[0].revents & POLLWRNORM))
#else
#error OS Not Implement.
#endif
        {
            iTmpLen = ::PUMP_CORE_SockSend(hSock, (char*)pBuf + iSendLen, dwBufSize - iSendLen);
            if (iTmpLen > 0)
            {
                iSendLen += iTmpLen;
                if (iSendLen == dwBufSize)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else if (iRet == 0)
        {
        }
        else
        {
            break;
        }
    } while (dwTimeOut > 0);

    return iSendLen;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSendExt(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    return PUMP_ERROR;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecv(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize)
{
    return ::recv(hSock, (char*)pBuf, dwBufSize, 0);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecvn(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut)
{
    int iRecvLen = 0;
    int iTmpLen = 0;
#if defined (PUMP_OS_WINDOWS)
    int iSelectRet = -1;
    timeval tv;
    fd_set fdset_r;

    if (dwTimeOut != PUMP_INFINITE)
    {
        tv.tv_sec = dwTimeOut / 1000;
        tv.tv_usec = (dwTimeOut % 1000) * 1000;
    }
#endif

    do
    {
#if defined (PUMP_OS_WINDOWS)
        FD_ZERO(&fdset_r);
        FD_SET(hSock, &fdset_r);

        if (dwTimeOut == PUMP_INFINITE)
        {
            iSelectRet = ::PUMP_CORE_Select((int)hSock + 1, &fdset_r, NULL, NULL, NULL);
        }
        else
        {
            iSelectRet = ::PUMP_CORE_Select((int)hSock + 1, &fdset_r, NULL, NULL, &tv);
            dwTimeOut = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        }

        if (iSelectRet > 0 && FD_ISSET(hSock, &fdset_r))
#elif defined (PUMP_OS_POSIX)
        struct pollfd fds[1] = { 0 };
        int iPollRet = -1;
        fds[0].fd = hSock;
        fds[0].events = POLLRDNORM;
        iPollRet = PUMP_CORE_Poll(fds, 1, (HPR_INT32*)&dwTimeOut);
        if ((iPollRet > 0) && (fds[0].revents & POLLRDNORM))
#else
#error OS Not Implement.
#endif
        {
            iTmpLen = ::PUMP_CORE_SockRecv(hSock, (char*)pBuf + iRecvLen, dwBufSize - iRecvLen);
            if (iTmpLen > 0)
            {
                iRecvLen += iTmpLen;
                if (iRecvLen == dwBufSize)//855gm
                {
                    break;
                }
            }
            else
            {
                //Debug("schina !!! PUMP_CORE_SockRecvn ::HPR_Recv return -1, errno = %d\n", errno);
                return -1;
            }
        }
#if defined (OS_WINDOWS)
    else if (iSelectRet == 0)
    {
    }
#elif defined (PUMP_OS_POSIX)
    else if (iPollRet == 0)
    {

    }
#endif
    else
    {
        //Debug("schina !!! PUMP_CORE_SockRecvn select|poll return timeout, return-1, errno = %d\n", errno);
        return -1;
    }

    } while (dwTimeOut > 0);

    //Debug("schina PUMP_CORE_SockRecvn Debug, return %d, and need %d\n", iRecvLen, dwBufSize);
    return iRecvLen;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecvExt(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    int iRecvLen = 0;
    int iTmpLen = 0;
#if defined (PUMP_OS_WINDOWS)
    int iSelectRet = -1;
    timeval tv;
    fd_set fdset_r;

    if (dwTimeout != PUMP_INFINITE)
    {
        tv.tv_sec = dwTimeout / 1000;
        tv.tv_usec = (dwTimeout % 1000) * 1000;
    }
#endif

#if defined (PUMP_OS_WINDOWS)
    FD_ZERO(&fdset_r);
    FD_SET(hSock, &fdset_r);

    if (dwTimeout == PUMP_INFINITE)
    {
        iSelectRet = ::PUMP_CORE_Select((int)hSock + 1, &fdset_r, NULL, NULL, NULL);
    }
    else
    {
        iSelectRet = ::PUMP_CORE_Select((int)hSock + 1, &fdset_r, NULL, NULL, &tv);
    }

    if (iSelectRet > 0 && FD_ISSET(hSock, &fdset_r))
#elif defined (PUMP_OS_POSIX)
    struct pollfd fds[1] = { 0 };
    int iPollRet = -1;
    fds[0].fd = hSock;
    fds[0].events = POLLRDNORM;
    iPollRet = PUMP_CORE_Poll(fds, 1, (pump_int32_t*)&dwTimeout);
    if ((iPollRet > 0) && (fds[0].revents & POLLRDNORM))
#else
#error OS Not Implement.
#endif
    {
        iTmpLen = ::PUMP_CORE_SockRecv(hSock, (char*)pBuf + iRecvLen, dwBufSize - iRecvLen);
        if (iTmpLen > 0)
        {
            return iTmpLen;
        }
        else
        {
            //Debug("schina !!! PUMP_CORE_SockRecvn ::HPR_Recv return -1, errno = %d\n", errno);
            return -1;
        }
    }
#if defined (OS_WINDOWS)
    else if (iSelectRet == 0)
    {
        return -1;
    }
#elif defined (PUMP_OS_POSIX)
    else if (iPollRet == 0)
    {
        return -1;
    }
#endif
    else
    {
        //Debug("schina !!! PUMP_CORE_SockRecvn select|poll return timeout, return-1, errno = %d\n", errno);
        return -1;
    }

    //return -1;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSendTo(pump_sock_t hSock, const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, const PUMP_ADDR * pLocalAddr)
{
    sockaddr_in sin4_ = pLocalAddr->sa_.sin4_;
    if (sin4_.sin_family == AF_INET)
    {
        return ::sendto(hSock, (char*)pBuf, dwBufSize, 0, (PSOCKADDR)&pLocalAddr->sa_, sizeof(pLocalAddr->sa_.sin4_));
    }
    else
    {
        return ::sendto(hSock, (char*)pBuf, dwBufSize, 0, (PSOCKADDR)&pLocalAddr->sa_, sizeof(pLocalAddr->sa_.sin6_));
    }
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockRecvFrom(pump_sock_t hSock, pump_pvoid_t pBuf, pump_uint32_t dwBufSize, PUMP_ADDR * pLocalAddr)
{
    int iAddrSize = sizeof(PUMP_ADDR);
    return ::recvfrom(hSock, (char*)pBuf, dwBufSize, 0, (PSOCKADDR)&pLocalAddr->sa_, (socklen_t*)&iAddrSize);

}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockBroadCastOn(pump_sock_t hSock)
{
    pump_bool_t bYes = PUMP_TRUE;
    return ::setsockopt(hSock, SOL_SOCKET, SO_BROADCAST, (char*)&bYes, sizeof(bYes));

}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockJoinMultiCastGroup(pump_sock_t hSock, const PUMP_ADDR * pLocalAddr, const PUMP_ADDR * pMCGroupAddr)
{
    if (!pLocalAddr || !pMCGroupAddr)
    {
        return PUMP_ERROR;
    }

    sockaddr_in McastGroupAddrSin4 = pMCGroupAddr->sa_.sin4_;
    pump_sockaddr_in6 McastGroupAddrSin6 = pMCGroupAddr->sa_.sin6_;
    sockaddr_in LocalSin4 = pLocalAddr->sa_.sin4_;

    PSOCKADDR pSockAddr = (PSOCKADDR)&(pMCGroupAddr->sa_);

    if (pSockAddr->sa_family == AF_INET)
    {
        struct ip_mreq struMerq = { 0 };
        struMerq.imr_multiaddr.s_addr = McastGroupAddrSin4.sin_addr.s_addr;
        struMerq.imr_interface.s_addr = LocalSin4.sin_addr.s_addr;
        return ::setsockopt(hSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&struMerq, sizeof(struMerq));
    }
#if defined (PUMP_OS_SUPPORT_IPV6)
    else if (pSockAddr->sa_family == AF_INET6)
#if 1//(defined(WIN32)/* && (_WIN32_WINNT > 0x0500)*/)
    {
        struct ipv6_mreq struMerq6;
        struMerq6.ipv6mr_interface = McastGroupAddrSin4.sin_addr.s_addr;
        memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, McastGroupAddrSin6.sin6_addr.s6_addr, 16);
#if defined(PUMP_OS_APPLE)
        return ::setsockopt(hSock, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&struMerq6, sizeof(struMerq6));
#else
        return ::setsockopt(hSock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*)&struMerq6, sizeof(struMerq6));
#endif
    }
#else
    {
        return PUMP_ERROR;
    }
#endif

#endif
    else
    {
        return PUMP_ERROR;
    }
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockLeaveMultiCastGroup(pump_sock_t hSock, const PUMP_ADDR * pLocalAddr, const PUMP_ADDR * pMCGroupAddr)
{
    if (!pLocalAddr || !pMCGroupAddr)
    {
        return PUMP_ERROR;
    }

    sockaddr_in McastGroupAddrSin4 = pMCGroupAddr->sa_.sin4_;
    pump_sockaddr_in6 McastGroupAddrSin6 = pMCGroupAddr->sa_.sin6_;
    sockaddr_in LocalSin4 = pLocalAddr->sa_.sin4_;

    PSOCKADDR pSockAddr = (PSOCKADDR)&(pMCGroupAddr->sa_);
    if (pSockAddr->sa_family == AF_INET)
    {
        struct ip_mreq struMerq;
        struMerq.imr_multiaddr.s_addr = McastGroupAddrSin4.sin_addr.s_addr;
        struMerq.imr_interface.s_addr = LocalSin4.sin_addr.s_addr;
        //InetPton(iAf,pAddr,&struMerq.imr_multiaddr);
        //InetPton(iAfLocal,pLocalAddrAddr,&struMerq.imr_interface);
        return ::setsockopt(hSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&struMerq, sizeof(struMerq));
    }
#if defined (PUMP_OS_SUPPORT_IPV6)
    else if (pSockAddr->sa_family == AF_INET6)
    {
        struct ipv6_mreq struMerq6;
        struMerq6.ipv6mr_interface = McastGroupAddrSin4.sin_addr.s_addr;
        memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, McastGroupAddrSin6.sin6_addr.s6_addr, 16);
#if defined(PUMP_OS_APPLE)
        return setsockopt(hSock, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char*)&struMerq6, sizeof(struMerq6));
#else
        return setsockopt(hSock, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char*)&struMerq6, sizeof(struMerq6));
#endif
    }
#endif
    else
    {
        return PUMP_ERROR;
    }
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetNonBlock(pump_sock_t hSock, pump_bool_t bBlock)
{
#if defined (PUMP_OS_WINDOWS)
    return ::ioctlsocket(hSock, (long)FIONBIO, (u_long*)&bBlock);
#else
    int flags = fcntl(hSock, F_GETFL, 0);
    if (bBlock)
    {
        return (fcntl(hSock, F_SETFL, flags | O_NONBLOCK) == -1) ? PUMP_ERROR : PUMP_OK;
    }
    else
    {
        return (fcntl(hSock, F_SETFL, flags &(~O_NONBLOCK)) == -1) ? PUMP_ERROR : PUMP_OK;
    }
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetTTL(pump_sock_t hSock, pump_int32_t * iTTL)
{
    int iSize = sizeof(iTTL);
    return ::getsockopt(hSock, IPPROTO_IP, IP_TTL, (char*)iTTL, (socklen_t*)&iSize);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetTTL_V6(pump_sock_t hSock, pump_int32_t* iTTL)
{
#if defined (PUMP_OS_SUPPORT_IPV6)
    int iSize = sizeof(iTTL);
    return ::getsockopt(hSock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char*)iTTL, (socklen_t*)&iSize);
#else
    return PUMP_ERROR;
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTTL(pump_sock_t hSock, pump_int32_t iTTL)
{
    return ::setsockopt(hSock, IPPROTO_IP, IP_TTL, (char*)&iTTL, sizeof(iTTL));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTTL_V6(pump_sock_t hSock, pump_int32_t iTTL)
{
#if defined (PUMP_OS_SUPPORT_IPV6)
    return ::setsockopt(hSock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char*)&iTTL, sizeof(iTTL));
#else
    return PUMP_ERROR;
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetMultiCastTTL(pump_sock_t hSock, pump_int32_t iTTL)
{
    return ::setsockopt(hSock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&iTTL, sizeof(iTTL));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetMultiCastTTL_V6(pump_sock_t hSock, pump_int32_t iTTL)
{
#if defined (PUMP_OS_SUPPORT_IPV6)
    return ::setsockopt(hSock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&iTTL, sizeof(iTTL));
#else
    return PUMP_ERROR;
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTOS(pump_sock_t hSock, pump_int32_t iTOS)
{
    return ::setsockopt(hSock, IPPROTO_IP, IP_TOS, (char*)&iTOS, sizeof(iTOS));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetTOS(pump_sock_t hSock, pump_int32_t * iTOS)
{
    int iSize = sizeof(*iTOS);
    return ::getsockopt(hSock, IPPROTO_IP, IP_TOS, (char*)iTOS, (socklen_t*)&iSize);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetReuseAddr(pump_sock_t hSock, pump_bool_t bReuse)
{
    return ::setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof(bReuse));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetTimeout(pump_sock_t hSock, pump_int32_t iSendTimeout, pump_int32_t iRecvTimeout)
{
    pump_int32_t iRet1 = PUMP_OK;
    pump_int32_t iRet2 = PUMP_OK;

#if defined (PUMP_OS_WINDOWS)
    if (iRecvTimeout)
    {
        iRet1 = ::setsockopt(hSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iRecvTimeout, sizeof(iRecvTimeout));
    }
    if (iSendTimeout)
    {
        iRet2 = ::setsockopt(hSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&iSendTimeout, sizeof(iSendTimeout));
    }
#elif defined(PUMP_OS_POSIX)
    if (iRecvTimeout)
    {
        timeval tv = { iRecvTimeout / 1000, iRecvTimeout % 1000 };
        iRet1 = setsockopt(hSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    }
    if (iSendTimeout)
    {
        timeval tv = { iSendTimeout / 1000, iSendTimeout % 1000 };
        iRet2 = setsockopt(hSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
    }
#endif

    return (iRet1 == PUMP_OK && iRet2 == PUMP_OK) ? PUMP_OK : PUMP_ERROR;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetBuffSize(pump_sock_t hSock, pump_int32_t iSendBuffSize, pump_int32_t iRecvBuffSize)
{
    pump_int32_t iRet1 = PUMP_OK;
    pump_int32_t iRet2 = PUMP_OK;

    if (iRecvBuffSize)
    {
        iRet1 = ::setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&iRecvBuffSize, sizeof(iRecvBuffSize));
    }
    if (iSendBuffSize)
    {
        iRet2 = ::setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&iSendBuffSize, sizeof(iSendBuffSize));
    }

    return (iRet1 == PUMP_OK && iRet2 == PUMP_OK) ? PUMP_OK : PUMP_ERROR;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockGetBuffSize(pump_sock_t hSock, pump_int32_t * iSendBuffSize, pump_int32_t * iRecvBuffSize)
{
    pump_int32_t iRet1 = PUMP_OK;
    pump_int32_t iRet2 = PUMP_OK;
    pump_int32_t iSize1 = sizeof(pump_int32_t);
    pump_int32_t iSize2 = sizeof(pump_int32_t);

    if (iRecvBuffSize)
    {
        iRet1 = ::getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)iRecvBuffSize, (socklen_t*)&iSize1);
    }
    if (iSendBuffSize)
    {
        iRet2 = ::getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)iSendBuffSize, (socklen_t*)&iSize2);
    }

    return (iRet1 == PUMP_OK && iRet2 == PUMP_OK) ? PUMP_OK : PUMP_ERROR;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockLingerOff(pump_sock_t hSock)
{
    linger lin = { 0 };
    lin.l_onoff = 0;
    return ::setsockopt(hSock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(linger));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockLingerOn(pump_sock_t hSock, pump_int32_t iTimeoutSec)
{
    linger lin = { 0 };
    lin.l_onoff = 1;
    lin.l_linger = iTimeoutSec;
    return ::setsockopt(hSock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(linger));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SockSetNoDelay(pump_sock_t hSock, pump_bool_t bDelay)
{
    return ::setsockopt(hSock, IPPROTO_TCP, TCP_NODELAY, (char*)&bDelay, sizeof(pump_bool_t));

}

namespace Pump
{
namespace Core
{
namespace Net
{

CSock* CSock::CreateSock()
{
    CSock * pSock = new (std::nothrow) CSock();
    if (!pSock)
    {
        return NULL;
    }
    return pSock;
}

CSock* CSock::CreateSock(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto)
{
    CSock * pSock = new (std::nothrow) CSock();
    if (!pSock)
    {
        return NULL;
    }
    if (pSock->Open(iAf, iType, iProto)!=PUMP_OK)
    {
        delete pSock;
        return NULL;
    }
    return pSock;
}

pump_int32_t CSock::DestroySock(CSock* pSock)
{
    if (pSock)
    {
        delete pSock;
    }
    return PUMP_OK;
}

CSock::CSock()
    : CHandleBase(PUMP_HANDLE_SOCKET, PUMP_INVALID_SOCKHANDLE)
{
}

CSock::CSock(pump_handle_t sock, const CAddr & refAddr)
    : CHandleBase(PUMP_HANDLE_SOCKET, sock)
    , m_addr(refAddr)
{

}

CSock::~CSock()
{
    this->Close();
}

pump_int32_t CSock::Open(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto)
{
    if (m_handle != PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != PUMP_CORE_STATE_DEFAULT)
    {
        return PUMP_ERROR;
    }
    m_handle = (pump_handle_t)::socket(iAf, iType, iProto);
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        switch (PUMP_CORE_GetSystemError())
        {
        case WSANOTINITIALISED:
        case WSAENETDOWN:
        case WSAEAFNOSUPPORT:
        default:
            break;
        }
        return PUMP_ERROR;
    }
    m_objState.SetState(CSockState::PUMP_SOCK_CREATED);
    return PUMP_OK;
}

pump_int32_t CSock::Close(pump_bool_t bForce)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_LISTENED
        || m_objState.GetState() != CSockState::PUMP_SOCK_CONNECTED
        || m_objState.GetState() != CSockState::PUMP_SOCK_SHUTDOWNED)
    {
        return PUMP_ERROR;
    }
    if (bForce)
    {
        this->Shutdown(PUMP_SHUT_RW);
    }
    pump_int32_t ret = ::closesocket((pump_sock_t)m_handle);
    if (ret == PUMP_OK)
    {
        m_objState.SetState(CSockState::PUMP_SOCK_CLOSED);
    }
    return ret;
}

pump_int32_t CSock::Shutdown(pump_int32_t iHow)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_LISTENED
        || m_objState.GetState() != CSockState::PUMP_SOCK_CONNECTED)
    {
        return PUMP_ERROR;
    }
    pump_int32_t ret = ::shutdown((pump_sock_t)m_handle, iHow);
    if (ret == PUMP_OK)
    {
        m_objState.SetState(CSockState::PUMP_SOCK_SHUTDOWNED);
    }
    return ret;
}

pump_int32_t CSock::Bind(const CAddr & refAddr)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_CREATED)
    {
        return PUMP_ERROR;
    }
    if (!refAddr.Available())
    {
        return PUMP_ERROR;
    }
    PUMP_ADDR struAddr;
    memset(&struAddr, 0, sizeof(struAddr));
    refAddr.GetAddr(struAddr);
    pump_sockaddr_in struSin = struAddr.sa_.sin4_;
    if (struSin.sin_family == PUMP_AF_INET)
    {
        return ::bind((pump_sock_t)m_handle, (PSOCKADDR)&struAddr.sa_, sizeof(pump_sockaddr_in));
    }
    else
    {
        return ::bind((pump_sock_t)m_handle, (PSOCKADDR)&struAddr.sa_, sizeof(PUMP_ADDR));
    }
}

pump_int32_t CSock::Listen(pump_int32_t iBackLog)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_CREATED)
    {
        return PUMP_ERROR;
    }
    pump_int32_t ret = ::listen((pump_sock_t)m_handle, iBackLog);
    if (ret == PUMP_OK)
    {
        m_objState.SetState(CSockState::PUMP_SOCK_LISTENED);
    }
    return ret;
}

pump_int32_t CSock::ListenExt(const CAddr & refAddr, pump_int32_t iBackLog)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_CREATED)
    {
        return PUMP_ERROR;
    }
    if (this->Bind(refAddr)==PUMP_ERROR)
    {
        return PUMP_ERROR;
    }
    pump_int32_t ret = ::listen((pump_sock_t)m_handle, iBackLog);
    if (ret == PUMP_OK)
    {
        m_objState.SetState(CSockState::PUMP_SOCK_LISTENED);
    }
    return ret;
}

CSock* CSock::Accept()
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_LISTENED)
    {
        return NULL;
    }
    pump_handle_t hAcceptSock = PUMP_INVALID_SOCKHANDLE;
    PUMP_ADDR struAddr;
    memset(&struAddr, 0, sizeof(struAddr));
    size_t dwAddrSize = sizeof(struAddr.sa_);
    hAcceptSock = (pump_handle_t)::accept((pump_sock_t)m_handle, (sockaddr*)&struAddr.sa_, (socklen_t*)&dwAddrSize);
    CAddr objAddr;
    objAddr.SetAddr(struAddr);
    CSock* pClientSock = new (std::nothrow)CSock(hAcceptSock, objAddr);
    if (pClientSock)
    {
        pClientSock->m_objState.SetState(CSockState::PUMP_SOCK_CONNECTED);
    }
    return pClientSock;
}

CSock* CSock::AcceptExt(pump_uint32_t dwTimeOut)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_LISTENED)
    {
        return NULL;
    }
    pump_handle_t hAcceptSock = PUMP_INVALID_SOCKHANDLE;
    PUMP_ADDR struAddr;
    memset(&struAddr, 0, sizeof(struAddr));
    size_t dwAddrSize = sizeof(struAddr.sa_);
    if (dwTimeOut == PUMP_INFINITE)
    {
        hAcceptSock = (pump_handle_t)::accept((pump_sock_t)m_handle, (sockaddr*)&struAddr.sa_, (socklen_t*)&dwAddrSize);
        //if (ret==PUMP_OK)
        //{
        //    
        //}
        //else
        //{
        //    hAcceptSock = ::accept(m_handle, PUMP_NULL, PUMP_NULL);
        //}
    }
    else
    {
        fd_set fdset_r;
        timeval tv = { dwTimeOut / 1000, (dwTimeOut % 1000) * 1000 };
        int iSelectRet = PUMP_ERROR;

        FD_ZERO(&fdset_r);
        FD_SET((pump_sock_t)m_handle, &fdset_r);

        iSelectRet = PUMP_CORE_Select((pump_int32_t)m_handle + 1, &fdset_r, NULL, NULL, &tv);
        if (iSelectRet > 0 && FD_ISSET((pump_sock_t)m_handle, &fdset_r))
        {
            hAcceptSock = (pump_handle_t)::accept((pump_sock_t)m_handle, (sockaddr*)&struAddr.sa_, (socklen_t*)&dwAddrSize);
        }
    }

    if (hAcceptSock==PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_NULL;
    }
    CAddr objAddr;
    objAddr.SetAddr(struAddr);
    CSock* pClientSock = new (std::nothrow)CSock(hAcceptSock, objAddr);
    if (!pClientSock)
    {
        pClientSock->m_objState.SetState(CSockState::PUMP_SOCK_CONNECTED);
    }
    return pClientSock;
}

pump_int32_t CSock::Connect(const CAddr & refAddr)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_CREATED)
    {
        return NULL;
    }
    if (!refAddr.Available())
    {
        return PUMP_ERROR;
    }
    PUMP_ADDR struAddr;
    memset(&struAddr, 0, sizeof(struAddr));
    refAddr.GetAddr(struAddr);
    pump_int32_t ret = ::connect((pump_sock_t)m_handle, (PSOCKADDR)&struAddr.sa_, sizeof(pump_sockaddr_in));
    if (ret == PUMP_OK)
    {
        m_objState.SetState(CSockState::PUMP_SOCK_CONNECTED);
    }
    return ret;
}

pump_int32_t CSock::ConnectExt(const CAddr & refAddr, pump_uint32_t dwTimeOut)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE || m_objState.GetState() != CSockState::PUMP_SOCK_CREATED)
    {
        return NULL;
    }
    if (!refAddr.Available())
    {
        return PUMP_ERROR;
    }
    PUMP_ADDR struAddr;
    memset(&struAddr, 0, sizeof(struAddr));
    refAddr.GetAddr(struAddr);
    if (dwTimeOut==PUMP_INFINITE)
    {
        return ::connect((pump_sock_t)m_handle, (PSOCKADDR)&struAddr.sa_, sizeof(pump_sockaddr_in));
    }

    int iSystemLastError = 0;
    pump_int32_t iSelelctRet = -1;
    pump_int32_t iRet = -1;

    this->SetNonBlock(PUMP_TRUE);
    sockaddr_in sin4_ = struAddr.sa_.sin4_;
    if (sin4_.sin_family == PUMP_AF_INET)
    {
        iRet = ::connect((pump_sock_t)m_handle, (PSOCKADDR)&struAddr.sa_.sin4_, sizeof(struAddr.sa_.sin4_));
    }
    else
    {
        iRet = ::connect((pump_sock_t)m_handle, (PSOCKADDR)&struAddr.sa_.sin6_, sizeof(struAddr.sa_.sin6_));
    }

    if (iRet == 0)
    {
        return iRet;
    }
#ifdef PUMP_OS_WINDOWS
    if (PUMP_CORE_GetSystemError() != WSAEWOULDBLOCK)
#elif (defined PUMP_OS_POSIX)
    if (CORE_INNER_GetSystemError() != EINPROGRESS)
#endif // PUMP_OS_WINDOWS
    {
        return iRet;
    }

    fd_set fdset_w;
    timeval tv = { dwTimeOut / 1000, (dwTimeOut % 1000) * 1000 };
    int iSelectRet = PUMP_ERROR;

    FD_ZERO(&fdset_w);
    FD_SET((pump_sock_t)m_handle, &fdset_w);

    iSelectRet = PUMP_CORE_Select((pump_int32_t)m_handle + 1, NULL, &fdset_w, NULL, &tv);
    if (iSelectRet > 0 && FD_ISSET(m_handle, &fdset_w))
    {
        pump_int32_t iError = 0;
        pump_int32_t iErrorLen;
        iErrorLen = sizeof(iError);
        pump_int32_t iTmp = ::getsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_ERROR, (char*)iError, (socklen_t*)&iErrorLen);
        if ((iTmp == 0) && (iError == 0))
        {
            iRet = 0;
        }
        else
        {
            if (iTmp == 0)
            {
                iSystemLastError = iError;
            }
            else
            {
                iSystemLastError = PUMP_CORE_GetSystemError();
            }
        }
    }
    else if (iSelectRet<0)
    {
        iSystemLastError = PUMP_CORE_GetSystemError();
#if (defined PUMP_OS_POSIX)
        if (dwTimeOut == 0)
        {
            iSystemLastError = ETIMEDOUT;
        }
#endif // PUMP_OS_POSIX
    }
    else // iSelectRet == 0
    {
#if (defined PUMP_OS_WINDOWS)
        iSystemLastError = WSAETIMEDOUT;
#elif (defined PUMP_OS_POSIX)
        iSystemLastError = ETIMEDOUT;
#endif // PUMP_OS_POSIX
    }

    this->SetNonBlock(PUMP_FALSE);
    if (iRet != 0)
    {
        PUMP_CORE_SetSystemError(iSystemLastError);
    }
    return iRet;
}

pump_int32_t CSock::BroadCastOn()
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    pump_bool_t bSwitch = PUMP_TRUE;
    return ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_BROADCAST, (char*)&bSwitch, sizeof(bSwitch));
}

pump_int32_t CSock::JoinMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    PUMP_ADDR struMGAddr, struLAddr;
    refMCGroupAddr.GetAddr(struMGAddr);
    refLocalAddr.GetAddr(struLAddr);
    pump_sockaddr_in struMcastGropAddrSin4 = struMGAddr.sa_.sin4_;
    pump_sockaddr_in6 struMcastGropAddrSin6 = struMGAddr.sa_.sin6_;
    pump_sockaddr_in struLSin4 = struLAddr.sa_.sin4_;
    PSOCKADDR pSockAddr = (PSOCKADDR)&(struMGAddr.sa_);
    if (pSockAddr->sa_family == PUMP_AF_INET)
    {
        struct ip_mreq struMerq = { 0 };
        struMerq.imr_multiaddr.s_addr = struMcastGropAddrSin4.sin_addr.s_addr;
        struMerq.imr_interface.s_addr = struLSin4.sin_addr.s_addr;
        return ::setsockopt((pump_sock_t)m_handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&struMerq, sizeof(struMerq));
    }
#if (defined PUMP_OS_SUPPORT_IPV6)
    else if (pSockAddr->sa_family == PUMP_AF_INET6)
    {
        struct ipv6_mreq struMerq6 = { 0 };
        struMerq6.ipv6mr_interface = struMcastGropAddrSin4.sin_addr.s_addr;
        memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, struMcastGropAddrSin6.sin6_addr.s6_addr, 16);
        return ::setsockopt((pump_sock_t)m_handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&struMerq6, sizeof(struMerq6));
    }
#endif // (defined PUMP_OS_SUPPORT_IPV6)
    else
    {
        return PUMP_ERROR;
    }
}

pump_int32_t CSock::LeaveMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CSock::SetNonBlock(pump_bool_t bBlock)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
#ifdef PUMP_OS_WINDOWS
    return ::ioctlsocket((pump_sock_t)m_handle, (pump_long_t)FIONBIO, (pump_ulong_t*)&bBlock);
#elif (defined PUMP_OS_POSIX)
    pump_int32_t iFlags = ::fcntl((pump_sock_t)m_handle, F_GETFL, 0);
    if (bBlock)
    {
        return ((::fcntl((pump_sock_t)m_handle, F_SETFL, iFlags | O_NONBLOCK) == -1) ? PUMP_ERROR:PUMP_OK);
    }
    else
    {
        return ((::fcntl((pump_sock_t)m_handle, F_SETFL, iFlags & (~O_NONBLOCK)) == -1) ? PUMP_ERROR : PUMP_OK);
    }
#endif // PUMP_OS_WINDOWS
    return PUMP_OK;
}

pump_int32_t CSock::GetTTL(pump_int32_t & iTTL)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    pump_int32_t iSize = sizeof(iTTL);
    return ::getsockopt((pump_sock_t)m_handle, IPPROTO_IP, IP_TTL, (char*)iTTL, (socklen_t*)&iSize);
}

pump_int32_t CSock::SetTTL(pump_int32_t iTTL)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return ::setsockopt((pump_sock_t)m_handle, IPPROTO_IP, IP_TTL, (char*)iTTL, sizeof(iTTL));
}

pump_int32_t CSock::SetMultiCastTTL(pump_int32_t iTTL)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CSock::SetTOS(pump_int32_t iTOS)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return ::setsockopt((pump_sock_t)m_handle, IPPROTO_IP, IP_TOS, (char*)iTOS, sizeof(iTOS));
}

pump_int32_t CSock::GetTOS(pump_int32_t & iTOS)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    pump_int32_t iSize = sizeof(iTOS);
    return ::getsockopt((pump_sock_t)m_handle, IPPROTO_IP, IP_TOS, (char*)iTOS, (socklen_t*)&iSize);
}

pump_int32_t CSock::SetReuseAddr(pump_bool_t bReuse)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return ::setsockopt((pump_sock_t)m_handle, IPPROTO_IP, SO_REUSEADDR, (char*)bReuse, sizeof(bReuse));
}

pump_int32_t CSock::SetTimeout(pump_int32_t iSendTimeout, pump_int32_t iRecvTimeout)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    pump_int32_t iRetRecv = PUMP_OK;
    pump_int32_t iRetSend = PUMP_OK;
#if (defined PUMP_OS_WINDOWS)
    if (iSendTimeout)
    {
        iRetSend = ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_SNDTIMEO, (char*)&iSendTimeout, sizeof(iSendTimeout));
    }
    if (iRecvTimeout)
    {
        iRetRecv = ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_RCVTIMEO, (char*)&iRecvTimeout, sizeof(iRecvTimeout));
    }
#elif (defined PUMP_OS_POSIX)
    if (iSendTimeout)
    {
        timeval tv = { iSendTimeout / 1000, iSendTimeout % 1000 };
        iRetSend = ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
    }
    if (iRecvTimeout)
    {
        timeval tv = { iSendTimeout / 1000, iSendTimeout % 1000 };
        iRetRecv = ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    }
#endif // (defined PUMP_OS_WINDOWS)
    return ((iRetSend == PUMP_OK && iRetRecv == PUMP_OK) ? PUMP_OK : PUMP_ERROR);
}

pump_int32_t CSock::SetBuffSize(pump_int32_t iSendBuffSize, pump_int32_t iRecvBuffSize)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    pump_int32_t iRetRecv = PUMP_OK;
    pump_int32_t iRetSend = PUMP_OK;
#if (defined PUMP_OS_WINDOWS)
    if (iSendBuffSize)
    {
        iRetSend = ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_SNDBUF, (char*)&iSendBuffSize, sizeof(iSendBuffSize));
    }
    if (iRecvBuffSize)
    {
        iRetRecv = ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_RCVBUF, (char*)&iRecvBuffSize, sizeof(iRecvBuffSize));
    }
#endif // (defined PUMP_OS_WINDOWS)
    return ((iRetSend == PUMP_OK && iRetRecv == PUMP_OK) ? PUMP_OK : PUMP_ERROR);
}

pump_int32_t CSock::GetBuffSize(pump_int32_t & iSendBuffSize, pump_int32_t & iRecvBuffSize)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    pump_int32_t iRetRecv = PUMP_OK;
    pump_int32_t iRetSend = PUMP_OK;
    pump_int32_t iSizeRecv = sizeof(iRetRecv);
    pump_int32_t iSizeSend = sizeof(iRetSend);;
#if (defined PUMP_OS_WINDOWS)
    if (iSendBuffSize)
    {
        iRetSend = ::getsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_SNDBUF, (char*)&iSendBuffSize, (socklen_t*)&iSizeSend);
    }
    if (iRecvBuffSize)
    {
        iRetRecv = ::getsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_RCVBUF, (char*)&iRecvBuffSize, (socklen_t*)&iSizeRecv);
    }
#endif // (defined PUMP_OS_WINDOWS)
    return ((iRetSend == PUMP_OK && iRetRecv == PUMP_OK) ? PUMP_OK : PUMP_ERROR);
}

pump_int32_t CSock::LingerOff()
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    linger lin = { 0 };
    lin.l_onoff = 0;
    return ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));
}

pump_int32_t CSock::LingerOn(pump_int32_t iTimeoutSec)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    linger lin = { 0 };
    lin.l_onoff = 1;
    lin.l_linger = iTimeoutSec;
    return ::setsockopt((pump_sock_t)m_handle, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));
}

pump_int32_t CSock::SetNoDelay(pump_bool_t bDelay)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return ::setsockopt((pump_sock_t)m_handle, IPPROTO_TCP, TCP_NODELAY, (char*)&bDelay, sizeof(bDelay));;
}

pump_int32_t CSock::Send(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return ::send((pump_sock_t)m_handle, (char*)pBuf, dwBufSize, 0);
}

pump_int32_t CSock::Sendn(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CSock::SendExt(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CSock::Recv(pump_pvoid_t pBuf, pump_uint32_t dwBufSize)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return ::recv((pump_sock_t)m_handle, (char*)pBuf, dwBufSize, 0);
}

pump_int32_t CSock::Recvn(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CSock::RecvExt(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_int32_t CSock::SendTo(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, const CAddr & refLocalAddr)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    PUMP_ADDR struAddr;
    refLocalAddr.GetAddr(struAddr);
    pump_sockaddr_in sin4_ = struAddr.sa_.sin4_;
    if (sin4_.sin_family == PUMP_AF_INET)
    {
        return ::sendto((pump_sock_t)m_handle, (char*)pBuf, dwBufSize, 0, (PSOCKADDR)&struAddr.sa_, sizeof(struAddr.sa_.sin4_));
    } 
    else
    {
        return ::sendto((pump_sock_t)m_handle, (char*)pBuf, dwBufSize, 0, (PSOCKADDR)&struAddr.sa_, sizeof(struAddr.sa_.sin6_));
    }
}

pump_int32_t CSock::RecvFrom(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, CAddr & refLocalAddr)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
    PUMP_ADDR struAddr;
    refLocalAddr.GetAddr(struAddr);
    pump_int32_t iAddrSize = sizeof(struAddr);
    return ::recvfrom((pump_sock_t)m_handle, (char*)pBuf, dwBufSize, 0, (PSOCKADDR)&struAddr.sa_, (socklen_t*)&iAddrSize);
}

pump_int32_t CSock::Ioctl(pump_int32_t iCmd, ...)
{
    if (m_handle == PUMP_INVALID_SOCKHANDLE)
    {
        return PUMP_ERROR;
    }
//#ifdef PUMP_OS_WINDOWS
//    return ::ioctlsocket((pump_sock_t)m_handle,iCmd,(pump_ulong_t*))
//#elif (defined PUMP_OS_POSIX)
//#endif // PUMP_OS_WINDOWS
//    return ::ioctlsocket((pump_sock_t)m_handle,iCmd,(pump_ulong_t*))
//#elif (defined PUMP_OS_POSIX)
    return PUMP_OK;
}

pump_sock_t CSock::GetSock() const
{
    return (pump_sock_t)m_handle;
}

const CAddr * CSock::GetAddr() const
{
    return &m_addr;
}

CSockState::PUMP_SOCK_STATE CSock::GetSockState() const
{
    return (CSockState::PUMP_SOCK_STATE)m_objState.GetState();
}

pump_bool_t CSock::Available() const
{
    return (m_handle!=PUMP_INVALID_SOCKHANDLE);
}

CSockHandle* CSockHandle::CreateSock()
{
    CSockHandle * pHSock = new (std::nothrow) CSockHandle();
    if (!pHSock)
    {
        return NULL;
    }
    return pHSock;
}

CSockHandle* CSockHandle::CreateSock(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto)
{
    CSockHandle * pHSock = new (std::nothrow) CSockHandle();
    if (!pHSock)
    {
        return NULL;
    }
    if (pHSock->Open(iAf, iType, iProto) != PUMP_OK)
    {
        delete pHSock;
        return NULL;
    }
    return pHSock;
}

pump_int32_t CSockHandle::DestroySock(CSockHandle* pHSock)
{
    if (pHSock)
    {
        delete pHSock;
    }
    return PUMP_OK;
}

CSockHandle::CSockHandle()
    : ::Pump::Core::CHandleBase(PUMP_HANDLE_SOCKET)
{
    m_handle = new (std::nothrow) CSock();
}

CSockHandle::CSockHandle(CSock * pSock)
    : ::Pump::Core::CHandleBase(PUMP_HANDLE_SOCKET)
{
    m_handle = pSock;
}

CSockHandle::~CSockHandle()
{
    delete (CSock*)m_handle;
    m_handle = PUMP_NULL;
}

pump_int32_t CSockHandle::Open(pump_int32_t iAf, pump_int32_t iType, pump_int32_t iProto)
{
    if (m_handle == PUMP_NULL || m_handle ==PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Open(iAf, iType, iProto);
}

pump_int32_t CSockHandle::Close(pump_bool_t bForce)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Close(bForce);
}

pump_int32_t CSockHandle::Shutdown(pump_int32_t iHow)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Shutdown(iHow);
}

pump_int32_t CSockHandle::Bind(const CAddr & refAddr)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Bind(refAddr);
}

pump_int32_t CSockHandle::Listen(pump_int32_t iBackLog)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Listen(iBackLog);
}

pump_int32_t CSockHandle::ListenExt(const CAddr & refAddr, pump_int32_t iBackLog)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->ListenExt(refAddr, iBackLog);
}

CSockHandle* CSockHandle::Accept()
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_NULL;
    }
    CSockHandle* pHClientSock = new (std::nothrow)CSockHandle(((CSock*)m_handle)->Accept());
    return pHClientSock;
}

CSockHandle* CSockHandle::AcceptExt(pump_uint32_t dwTimeOut)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_NULL;
    }
    CSockHandle* pHClientSock = new (std::nothrow)CSockHandle(((CSock*)m_handle)->AcceptExt(dwTimeOut));
    return pHClientSock;
}

pump_int32_t CSockHandle::Connect(const CAddr & refAddr)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Connect(refAddr);
}

pump_int32_t CSockHandle::ConnectExt(const CAddr & refAddr, pump_uint32_t dwTimeOut)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->ConnectExt(refAddr, dwTimeOut);
}

pump_int32_t CSockHandle::Send(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Send(pBuf, dwBufSize);
}

pump_int32_t CSockHandle::Sendn(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Sendn(pBuf, dwBufSize, dwTimeOut);
}

pump_int32_t CSockHandle::SendExt(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SendExt(pBuf, dwBufSize, dwTimeout);
}

pump_int32_t CSockHandle::Recv(pump_pvoid_t pBuf, pump_uint32_t dwBufSize)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Recv(pBuf, dwBufSize);
}

pump_int32_t CSockHandle::Recvn(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeOut)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->Recvn(pBuf, dwBufSize, dwTimeOut);
}

pump_int32_t CSockHandle::RecvExt(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, pump_uint32_t dwTimeout)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->RecvExt(pBuf, dwBufSize, dwTimeout);
}

pump_int32_t CSockHandle::SendTo(const pump_pvoid_t pBuf, pump_uint32_t dwBufSize, const CAddr & refLocalAddr)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SendTo(pBuf, dwBufSize, refLocalAddr);
}

pump_int32_t CSockHandle::RecvFrom(pump_pvoid_t pBuf, pump_uint32_t dwBufSize, CAddr & refLocalAddr)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->RecvFrom(pBuf, dwBufSize, refLocalAddr);
}

pump_int32_t CSockHandle::BroadCastOn()
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->BroadCastOn();
}

pump_int32_t CSockHandle::JoinMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->JoinMultiCastGroup(refLocalAddr, refMCGroupAddr);
}

pump_int32_t CSockHandle::LeaveMultiCastGroup(const CAddr & refLocalAddr, const CAddr & refMCGroupAddr)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->LeaveMultiCastGroup(refLocalAddr, refMCGroupAddr);
}

pump_int32_t CSockHandle::SetNonBlock(pump_bool_t bBlock)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetNonBlock(bBlock);
}

pump_int32_t CSockHandle::GetTTL(pump_int32_t & iTTL)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->GetTTL(iTTL);
}

pump_int32_t CSockHandle::SetTTL(pump_int32_t iTTL)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetTTL(iTTL);
}

pump_int32_t CSockHandle::SetMultiCastTTL(pump_int32_t iTTL)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetMultiCastTTL(iTTL);
}

pump_int32_t CSockHandle::SetTOS(pump_int32_t iTOS)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetTOS(iTOS);
}

pump_int32_t CSockHandle::GetTOS(pump_int32_t & iTOS)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->GetTOS(iTOS);
}

pump_int32_t CSockHandle::SetReuseAddr(pump_bool_t bReuse)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetReuseAddr(bReuse);
}

pump_int32_t CSockHandle::SetTimeout(pump_int32_t iSendTimeout, pump_int32_t iRecvTimeout)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetTimeout(iSendTimeout, iRecvTimeout);
}

pump_int32_t CSockHandle::SetBuffSize(pump_int32_t iSendBuffSize, pump_int32_t iRecvBuffSize)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetBuffSize(iSendBuffSize, iRecvBuffSize);
}

pump_int32_t CSockHandle::GetBuffSize(pump_int32_t & iSendBuffSize, pump_int32_t & iRecvBuffSize)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->GetBuffSize(iSendBuffSize, iRecvBuffSize);
}

pump_int32_t CSockHandle::LingerOff()
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->LingerOff();
}

pump_int32_t CSockHandle::LingerOn(pump_int32_t iTimeoutSec)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->LingerOn(iTimeoutSec);
}

pump_int32_t CSockHandle::SetNoDelay(pump_bool_t bDelay)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->SetNoDelay(bDelay);
}

pump_int32_t CSockHandle::Ioctl(pump_int32_t iCmd, ...)
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return PUMP_OK;
}

pump_sock_t CSockHandle::GetSock() const
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    return ((CSock*)m_handle)->GetSock();
}

const CAddr * CSockHandle::GetAddr() const
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_NULL;
    }
    return ((CSock*)m_handle)->GetAddr();
}

CSockState::PUMP_SOCK_STATE CSockHandle::GetSockState() const
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return CSockState::PUMP_SOCK_DEFAULT;
    }
    return ((CSock*)m_handle)->GetSockState();
}

pump_bool_t CSockHandle::Available() const
{
    if (m_handle == PUMP_NULL || m_handle == PUMP_INVALID_HANDLE)
    {
        return PUMP_FALSE;
    }
    return ((CSock*)m_handle)->Available();
}


}
}
}