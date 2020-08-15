/**
 * @file pump_core_multiplex.h
 * @brief Library Header.
 * @details Declaration of multiplex backend interface.
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

#ifndef PUMP_EVENT_MULTIPLEXBACKEND_H
#define PUMP_EVENT_MULTIPLEXBACKEND_H
#include "pump_macro/pump_pre.h"
#include <cstring>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <stack>
#include "pump_core/network/pump_core_sock.h"

// multiplexing API
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Poll(LPPUMP_POLLFD pFds, pump_int32_t iFds, pump_int32_t iTimeOut);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Select(pump_int32_t iMaxFd, LPPUMP_FDSET fdset_r, LPPUMP_FDSET fdset_w, LPPUMP_FDSET fdset_e, timeval* tv);

namespace Pump
{
namespace Core
{
namespace Net
{

typedef enum tagPUMP_IOEV_TYPE
{
    PUMP_IOEV_NONE = 0x000,    ///< io fd no event
    PUMP_IOEV_IN = 0x001,      ///< io fd readable
    PUMP_IOEV_OUT = 0x002,     ///< io fd writeable
    PUMP_IOEV_ERR = 0x004   ///< io fd error
} PUMP_IOEV_TYPE;

/**
* @brief describe IoFd changing type.
*/
typedef enum tagPUMP_FDCTRL_TYPE
{
    PUMP_FDCTL_DEFAULT = 0, ///< default type
    PUMP_FDCTL_ADD,  ///< add listening pump event
    PUMP_FDCTL_DEL,  ///< del listening pump event
    PUMP_FDCTL_MOD   ///< mod listening pump event
} PUMP_FDCTRL_TYPE;

/**
 * @brief describe changing IoFd.
 */
class CFdCtl
{
public:
    PUMP_FDCTRL_TYPE type_;
    pump_fd_t fd_;
    pump_uint32_t fd_ev_;

    CFdCtl()
        : type_(PUMP_FDCTL_DEFAULT), fd_((pump_fd_t)-1), fd_ev_(PUMP_IOEV_NONE) {}
};

/**
 * @brief Result of calling pump poll API.
 */
class CFdRet
{
public:
    /**
     * @brief fd
     */
    pump_fd_t fd_;

    /**
     * @var pump_uint32_t fd_ev_
     * @brief event type, must contain in PUMP_IOEV_TYPE
     */
    pump_uint32_t re_fd_ev_;

    CFdRet()
        : fd_((pump_fd_t)-1), re_fd_ev_(PUMP_IOEV_NONE) {}
};

typedef std::vector<CFdRet> CFdRetList;

/**
 * @brief IO Multiplex Backend
 */
PUMP_ABSTRACT
class PUMP_CORE_CLASS CMultiplexBackend
{
public:
    CMultiplexBackend() {}

    virtual ~CMultiplexBackend() {}

    /**
     * @brief init
     * @return 0-succ, -1-fail
     */
    virtual int init() = 0;

    /**
     * @brief update listening pump event
     * @param [in] refChange changing pump event
     * @return 0-succ, -1-fail
     */
    virtual int update(const CFdCtl &refChange) = 0;

    /**
     * @fn int wait(CFdRetList &fdRetList, timeval & tmv) = 0
     * @brief waiting for pump event, call select()/poll() in this funtion
     * @param [out] refIoFdRet set of happend pump event
     * @param [in] tmv timeout value
     * @return succ-listening fd count, fail-1
     */
    virtual int wait(CFdRetList &refIoFdRet, timeval & tmv) = 0;
};

typedef enum tagPUMP_BACKEND_TYPE
{
    PUMP_SELECT,
    PUMP_POLL,
    PUMP_EPOLL,
} PUMP_BACKEND_TYPE;

/**
 * @brief select model
 */
PUMP_IMPLEMENT
class PUMP_CORE_CLASS CSelect
    : public CMultiplexBackend
{
public:
    typedef struct
    {
        PUMP_FDSET fd_read_;
        PUMP_FDSET fd_write_;
        PUMP_FDSET fd_error_;
    } PUMP_FDSETS;
public:
    CSelect();

    ~CSelect();

    virtual int init();

    virtual int update(const CFdCtl &refChange);

    virtual int wait(CFdRetList &refIoFdRet, timeval & tmv);

private:
    /**
     * @brief restore m_setWork from m_setBackup
     */
    void recover();

    /**
     * @brief get max fd in m_fds
     * @return maximum fd
     */
    pump_fd_t Max();

private:
    /**
     * @brief set of working fd.
     */
    PUMP_FDSETS m_setWork;
    /**
     * @brief set of backup fd.
     */
    PUMP_FDSETS m_setBackup;
    /**
     * @brief all listening fd.
     */
    std::set<pump_fd_t> m_fds;
};

PUMP_IMPLEMENT
class PUMP_CORE_CLASS CPoll
    : public CMultiplexBackend
{
public:
    CPoll() {}
};

PUMP_IMPLEMENT
class PUMP_CORE_CLASS CEpoll
    : public CMultiplexBackend
{
public:
    CEpoll() {}
};

}
}
}

#endif //PUMP_EVENT_MULTIPLEXBACKEND_H
