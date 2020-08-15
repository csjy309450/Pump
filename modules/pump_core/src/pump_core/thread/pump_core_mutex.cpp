/**
 * @file pump_core_mutex.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of mutex control interface.
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

#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/pump_core_types.h"
#include <cassert>
#include <algorithm>
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>

namespace Pump
{
namespace Core
{
namespace Thread
{
class CMutexPrimitive
    : public __CPrimitiveBase
{
public:
    boost::mutex m_mux;
};

CMutex::CMutex()
    : CObjectBase(new CMutexPrimitive())
{
    
}

CMutex::~CMutex()
{
    
}

void CMutex::Lock()
{
    if (!m_pPrimitive)
    {
        assert(1 == 0);
    }
    CMutexPrimitive * primitive = dynamic_cast<CMutexPrimitive*>(m_pPrimitive);
    primitive->m_mux.lock();
}

bool CMutex::TryLock()
{
    if (!m_pPrimitive)
    {
        assert(1 == 0);
    }
    CMutexPrimitive * primitive = dynamic_cast<CMutexPrimitive*>(m_pPrimitive);
    return primitive->m_mux.try_lock();
}

void CMutex::Unlock()
{
    if (!m_pPrimitive)
    {
        assert(1 == 0);
    }
    CMutexPrimitive * primitive = dynamic_cast<CMutexPrimitive*>(m_pPrimitive);
    return primitive->m_mux.unlock();
}

CMutexGuilder::CMutexGuilder(CMutex & locker)
    : m_locker(locker)
{
    m_locker.Lock();
}

CMutexGuilder::~CMutexGuilder()
{
    m_locker.Unlock();
}


PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1)
{
    pump_handle_t hMux[2] = {
        (pump_handle_t)&mux0,
        (pump_handle_t)&mux1 };
    std::sort(hMux, hMux + 2);
    for (int i = 0; i < 2; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Lock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2)
{
    pump_handle_t hMux[3] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2};
    std::sort(hMux, hMux+3);
    for (int i = 0; i < 3; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Lock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3)
{
    pump_handle_t hMux[4] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3};
    std::sort(hMux, hMux+4);
    for (int i = 0; i < 4; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Lock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4)
{
    pump_handle_t hMux[5] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3,
                             (pump_handle_t)&mux4};
    std::sort(hMux, hMux+5);
    for (int i = 0; i < 5; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Lock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5)
{
    pump_handle_t hMux[6] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3,
                             (pump_handle_t)&mux4,
                             (pump_handle_t)&mux5};
    std::sort(hMux, hMux+6);
    for (int i = 0; i < 6; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Lock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5, CMutex &mux6)
{
    pump_handle_t hMux[7] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3,
                             (pump_handle_t)&mux4,
                             (pump_handle_t)&mux5,
                             (pump_handle_t)&mux6};
    std::sort(hMux, hMux+7);
    for (int i = 0; i < 7; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Lock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1)
{
    pump_handle_t hMux[2] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1};
    std::sort(hMux, hMux+2);
    for (int i = 0; i < 2; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Unlock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2)
{
    pump_handle_t hMux[3] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2};
    std::sort(hMux, hMux+3);
    for (int i = 0; i < 3; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Unlock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3)
{
    pump_handle_t hMux[4] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3};
    std::sort(hMux, hMux+4);
    for (int i = 0; i < 4; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Unlock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4)
{
    pump_handle_t hMux[5] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3,
                             (pump_handle_t)&mux4};
    std::sort(hMux, hMux+5);
    for (int i = 0; i < 5; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Unlock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5)
{
    pump_handle_t hMux[6] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3,
                             (pump_handle_t)&mux4,
                             (pump_handle_t)&mux5};
    std::sort(hMux, hMux+6);
    for (int i = 0; i < 6; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Unlock();
    }
}

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5, CMutex &mux6)
{
    pump_handle_t hMux[7] = {(pump_handle_t)&mux0,
                             (pump_handle_t)&mux1,
                             (pump_handle_t)&mux2,
                             (pump_handle_t)&mux3,
                             (pump_handle_t)&mux4,
                             (pump_handle_t)&mux5,
                             (pump_handle_t)&mux6};
    std::sort(hMux, hMux+7);
    for (int i = 0; i < 7; ++i)
    {
        CMutex & t_mux = *(CMutex*)hMux[i];
        t_mux.Unlock();
    }
}

}
}
}