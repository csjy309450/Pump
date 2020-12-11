/**
 * @file pump_core_multiplex.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of  multiplex backend interface.
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

#include "pump_macro/pump_pre.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/network/pump_core_sock.h"
#include "pump_core/network/pump_core_multiplex.h"

pump_int32_t PUMP_CORE_Select(pump_int32_t iMaxFd, LPPUMP_FDSET fdset_r, LPPUMP_FDSET fdset_w, LPPUMP_FDSET fdset_e, timeval* tv)
{
#if (defined PUMP_OS_WINDOWS)
    return ::select(iMaxFd, fdset_r, fdset_w, fdset_e, tv);
#elif (defined PUMP_OS_POSIX)
    timeval t;
    if (!tv)
    {
        return ::select(iMaxFd, fdset_r, fdset_w, fdset_e, PUMP_NULL);
    }
    t = *tv;
    return ::select(iMaxFd, fdset_r, fdset_w, fdset_e, &t);
#endif // PUMP_OS_WINDOWS
}

pump_int32_t PUMP_CORE_Poll(LPPUMP_POLLFD pFds, pump_int32_t iFds, pump_int32_t iTimeOut)
{
#if (defined PUMP_OS_WINDOWS)
#if(_WIN32_WINNT >= 0x0600)
    return WSAPoll(pFds, iFds, iTimeOut);
#else
    return PUMP_ERROR;
#endif // (_WIN32_WINNT >= 0x0600)
#elif (defined PUMP_OS_POSIX)
    return ::poll(pFds, iFds, iTimeOut);
#endif // PUMP_OS_WINDOWS
}

namespace Pump
{
namespace Core
{
namespace Net
{

CSelect::CSelect()
{
    init();
}

CSelect::~CSelect()
{

}

int CSelect::init()
{
    FD_ZERO(&m_setWork.fd_read_);
    FD_ZERO(&m_setWork.fd_write_);
    FD_ZERO(&m_setWork.fd_error_);
    FD_ZERO(&m_setBackup.fd_read_);
    FD_ZERO(&m_setBackup.fd_write_);
    FD_ZERO(&m_setBackup.fd_error_);
    return 0;
}

int CSelect::update(const CFdCtl &refChange)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("_select::update() in");
#endif //_TEST_LEVEL_DEBUG
    switch (refChange.type_)
    {
    case PUMP_FDCTL_ADD:
    {
        if (m_fds.size() > FD_SETSIZE)
        {
            goto ret_bad;
        }
        if (m_fds.find(refChange.fd_) != m_fds.end())
        {
            goto ret_bad;
        }
        if ((refChange.fd_ev_ & PUMP_IOEV_IN) != 0)
        {
            FD_SET((pump_sock_t)refChange.fd_, &m_setBackup.fd_read_);
        }
        if ((refChange.fd_ev_ & PUMP_IOEV_OUT) != 0)
        {
            FD_SET((pump_sock_t)refChange.fd_, &m_setBackup.fd_write_);
        }
        if ((refChange.fd_ev_ & PUMP_IOEV_ERR) != 0)
        {
            FD_SET((pump_sock_t)refChange.fd_, &m_setBackup.fd_error_);
        }
        // insert to fd set
        m_fds.insert(refChange.fd_);
        break;
    }
    case PUMP_FDCTL_DEL:
    {
        if (m_fds.find(refChange.fd_) == m_fds.end())
        {
            goto ret_bad;
        }
        FD_CLR((pump_sock_t)refChange.fd_, &m_setBackup.fd_read_);
        FD_CLR((pump_sock_t)refChange.fd_, &m_setBackup.fd_write_);
        FD_CLR((pump_sock_t)refChange.fd_, &m_setBackup.fd_error_);
        // remove from fd set
        m_fds.erase(refChange.fd_);
        break;
    }
    case PUMP_FDCTL_MOD:
    {
        if (m_fds.find(refChange.fd_) == m_fds.end()) // can't find fd
        {

            if (refChange.fd_ev_ != 0) // exist related io event, insert fd set.
            {
                m_fds.insert(refChange.fd_);
            }
            else
            {
                goto ret_bad;
            }
        }
        else // find fd
        {
            // inexistent related io event, delete fd set.
            if (refChange.fd_ev_ == 0)
            {
                m_fds.erase(refChange.fd_);
            }
        }

        // clear refChange.m_fd
        FD_CLR((pump_sock_t)refChange.fd_, &m_setBackup.fd_read_);
        FD_CLR((pump_sock_t)refChange.fd_, &m_setBackup.fd_write_);
        FD_CLR((pump_sock_t)refChange.fd_, &m_setBackup.fd_error_);
        // reset listening flag
        if ((refChange.fd_ev_ & PUMP_IOEV_IN) != 0)
        {
            FD_SET((pump_sock_t)refChange.fd_, &m_setBackup.fd_read_);
        }
        if ((refChange.fd_ev_ & PUMP_IOEV_OUT) != 0)
        {
            FD_SET((pump_sock_t)refChange.fd_, &m_setBackup.fd_write_);
        }
        if ((refChange.fd_ev_ & PUMP_IOEV_ERR) != 0)
        {
            FD_SET((pump_sock_t)refChange.fd_, &m_setBackup.fd_error_);
        }
        break;
    }
    default:
    {
        break;
    }
    }

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("_select::update() out with 0");
#endif //_TEST_LEVEL_DEBUG
    return 0;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("_select::update() out with -1");
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

pump_fd_t CSelect::Max()
{
    return *(std::max_element(m_fds.begin(), m_fds.end()));
}

int CSelect::wait(CFdRetList &refIoFdRet, timeval &tmv)
{
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("_select::wait() in");
#endif //_TEST_LEVEL_DEBUG

    int ret_count = 0;
    int ret_select = 0;
    // 1.listening set is empty, return
    if (m_fds.empty())
    {
#ifdef _TEST_LEVEL_DEBUG
        PUMP_CORE_INFO("_M_fds.empty() == true");
#endif //_TEST_LEVEL_DEBUG
        goto ret_good;
    }

    // 2.recover working fd_set from backup fd_set
    recover();
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("::select() will block %l(s) and %l(us)", tmv.tv_sec, tmv.tv_usec );
#endif //_TEST_LEVEL_DEBUG
    // 3.call ::select()
    ret_select = ::select((int)Max() + 1,
        &m_setWork.fd_read_,
        &m_setWork.fd_write_,
        &m_setWork.fd_error_,
        &tmv);
    if (ret_select == 0)
    { // select() timeout
        // TODO add timeout process
        goto ret_good;
    }
    else if (ret_select == -1)
    { // function error
        PUMP_CORE_INFO("[Error]: %d", PUMP_CORE_GetSystemError());
        // TODO add error process
        goto ret_bad;
    }
    else
    {
        for (std::set<pump_fd_t>::iterator it = m_fds.begin();
            it != m_fds.end(); ++it)
        { // scan socket in listening fd set
            int re_ev = PUMP_IOEV_NONE;
            if (FD_ISSET(*it, &m_setWork.fd_read_))
            { // fd have readable event
                re_ev |= PUMP_IOEV_IN;
            }
            if (FD_ISSET(*it, &m_setWork.fd_write_))
            { // fd have writeable event
                re_ev |= PUMP_IOEV_OUT;
            }
            if (FD_ISSET(*it, &m_setWork.fd_error_))
            { // fd have error event
                re_ev |= PUMP_IOEV_ERR;
            }

            if (re_ev == PUMP_IOEV_NONE)
            { // fd have unhappened any event
                continue;
            }
            else
            {
                CFdRet ioFdRet;
                ioFdRet.fd_ = *it;
                ioFdRet.re_fd_ev_ = re_ev;
                // put into list to return
                refIoFdRet.push_back(ioFdRet);
            }
        }
    }
    ret_count = static_cast<int>(m_fds.size());

ret_good:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("_select::wait() out with %d", ret_count);
#endif //_TEST_LEVEL_DEBUG
    return ret_count;

ret_bad:
#ifdef _TEST_LEVEL_DEBUG
    PUMP_CORE_INFO("_select::wait() out with %d", -1);
#endif //_TEST_LEVEL_DEBUG
    return -1;
}

void CSelect::recover()
{
    FD_ZERO(&m_setWork.fd_read_);
    FD_ZERO(&m_setWork.fd_write_);
    FD_ZERO(&m_setWork.fd_error_);
    ::memcpy(&m_setWork, &m_setBackup, sizeof(m_setBackup));
}

}
}
}