/**
 * @file pump_core_rwlocker.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of rw-locker interface.
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

#include "pump_core/thread/pump_core_rwlocker.h"
#ifdef __linux__
#include <unistd.h>
#elif (defined _WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <synchapi.h>
#endif // __linux__

namespace Pump
{
namespace Core
{
namespace Thread
{

CRWLocker::CRWLocker()
    : m_muxRead(),
      m_muxWrite(),
      m_nReader(0),
      m_wFalg(false) {}


CRWLocker::~CRWLocker()
{
}

void CRWLocker::readLock()
{
    m_muxWrite.Lock();
    m_muxRead.Lock();
    ++(m_nReader);
    m_muxRead.Unlock();
    m_muxWrite.Unlock();
}

void CRWLocker::readUnlock()
{
    m_muxWrite.Lock();
    m_muxRead.Lock();
    if ((m_nReader.Value()) > 0)
    {
        (--m_nReader);
    }
    m_muxRead.Unlock();
    m_muxWrite.Unlock();
}

void CRWLocker::writeLock()
{
    do
    {
        m_muxWrite.Lock();
        if ((m_nReader) == 0)
        {
            break;
        }
        m_muxWrite.Unlock();
#ifdef __linux__
        ::usleep(500);
#elif (defined _WIN32)
        ::Sleep(500);
#endif //__linux__
    } while (1);
    (m_wFalg) = true;
}

void CRWLocker::writeUnlock()
{
    if (!m_muxWrite.TryLock())
    {
        if (m_wFalg.Value())
        {
            m_muxWrite.Unlock();
        }
    } else
    {
        m_muxWrite.Unlock();
    }
}
    
}
}
}