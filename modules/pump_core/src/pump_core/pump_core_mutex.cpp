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

#include "pump_core/pump_core_mutex.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include <cassert>
#include <algorithm>
#ifdef PUMP_CORE_HAVE_BOOST
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#endif // PUMP_CORE_HAVE_BOOST

PUMP_CORE_API pump_int32_t PUMP_CORE_MutexCreate(pump_mutex_t* pMutex, pump_int32_t nFlag)
{
#if (defined PUMP_OS_WINDOWS)
    __try
    {
#if defined PUMP_OS_WINPHONE
        InitializeCriticalSectionEx(pMutex, 4000, 0);
#else
        InitializeCriticalSection(pMutex);
#endif
    }
    __except(GetExceptionCode() == STATUS_NO_MEMORY)
    {
        return PUMP_ERROR;
    }

    return PUMP_OK;
#elif (defined PUMP_OS_POSIX)
    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    if (nFlag != -1)
    {
        pthread_mutexattr_settype(&attr, nFlag);
    }
    else
    {
        pthread_mutexattr_settype(&attr, PUMP_MUTEX_RECURSIVE);
    }

    //pthread_mutex_init always return 0
    pthread_mutex_init(pMutex, &attr);
    pthread_mutexattr_destroy(&attr);

    return PUMP_OK;
#endif // (defined PUMP_OS_POSIX)
}

PUMP_CORE_API pump_int32_t PUMP_CORE_MutexDestroy(pump_mutex_t* pMutex)
{
#if (defined PUMP_OS_WINDOWS)
    if ( !pMutex )
    {
        return PUMP_ERROR;
    }

    DeleteCriticalSection(pMutex);
    return PUMP_OK;
#elif (defined PUMP_OS_POSIX)
    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    return pthread_mutex_destroy(pMutex) == 0 ? PUMP_OK : PUMP_ERROR;
#endif // (defined PUMP_OS_POSIX)
}

PUMP_CORE_API pump_int32_t PUMP_CORE_MutexLock(pump_mutex_t* pMutex)
{
#if (defined PUMP_OS_WINDOWS)
    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    EnterCriticalSection(pMutex);
    return PUMP_OK;
#elif (defined PUMP_OS_POSIX)
    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    return pthread_mutex_lock(pMutex) == 0 ? PUMP_OK : PUMP_ERROR;
#endif // (defined PUMP_OS_POSIX)
}

PUMP_CORE_API pump_int32_t PUMP_CORE_MutexTryLock(pump_mutex_t* pMutex)
{
#if (defined PUMP_OS_WINDOWS)
#   if (defined(WIN32) && _WIN32_WINNT >= 0x0500)

    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    return (TryEnterCriticalSection(pMutex)) ? PUMP_OK : PUMP_ERROR;
#   else
    return PUMP_ERROR;
#   endif
#elif (defined PUMP_OS_POSIX)
    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    return pthread_mutex_trylock(pMutex) == 0 ? PUMP_OK : PUMP_ERROR;
#endif // (defined PUMP_OS_POSIX)
}

PUMP_CORE_API pump_int32_t PUMP_CORE_MutexUnlock(pump_mutex_t* pMutex)
{
#if (defined PUMP_OS_WINDOWS)
    if ( !pMutex )
    {
        return PUMP_ERROR;
    }

    LeaveCriticalSection(pMutex);
    return PUMP_OK;
#elif (defined PUMP_OS_POSIX)
    if (!pMutex)
    {
        return PUMP_ERROR;
    }

    return pthread_mutex_unlock(pMutex) == 0 ? PUMP_OK : PUMP_ERROR;
#endif // (defined PUMP_OS_POSIX)
}

namespace Pump
{
namespace Core
{
namespace Thread
{
class CMutexPrimitive
    : public CPrimitiveBase
{
public:
    virtual void Lock() = 0;
    virtual bool TryLock() = 0;
    virtual void Unlock() = 0;
};

#ifdef PUMP_CORE_HAVE_BOOST
class CBoostMutexPrimitive
    : public CPrimitiveBase
{
public:
    CBoostMutexPrimitive() : CPrimitiveBase(), m_mux() {}
    virtual void Lock()
    {
        m_mux.lock();
    }
    virtual bool TryLock()
    {
        return m_mux.try_lock();
    }
    virtual void Unlock()
    {
        m_mux.unlock();
    }
private:
    boost::mutex m_mux;
};
#endif // PUMP_CORE_HAVE_BOOST

class CBuildinMutexPrimitive
    : public CPrimitiveBase
{
public:
    CBuildinMutexPrimitive() 
        : CPrimitiveBase() 
    {
       pump_int32_t ret = PUMP_CORE_MutexCreate(&m_mux);
    }
    virtual ~CBuildinMutexPrimitive()
    {
        PUMP_CORE_MutexDestroy(&m_mux);
    }
    virtual void Lock()
    {
        PUMP_CORE_MutexLock(&m_mux);
    }
    virtual bool TryLock()
    {
        return (PUMP_CORE_MutexTryLock(&m_mux) == PUMP_OK ? true : false);
    }
    virtual void Unlock()
    {
        PUMP_CORE_MutexUnlock(&m_mux);
    }
private:
    pump_mutex_t m_mux;
};

CMutex::CMutex()
#ifdef PUMP_CORE_HAVE_BOOST
    : CObjectBase("CMutex",PUMP_FALSE, new(std::nothrow) CBoostMutexPrimitive())
#else
    : CObjectBase("CMutex",PUMP_FALSE, new(std::nothrow) CBuildinMutexPrimitive())
#endif // PUMP_CORE_HAVE_BOOST
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
    CMutexPrimitive * primitive = static_cast<CMutexPrimitive*>(m_pPrimitive);
    primitive->Lock();
}

bool CMutex::TryLock()
{
    if (!m_pPrimitive)
    {
        assert(1 == 0);
    }
    CMutexPrimitive * primitive = static_cast<CMutexPrimitive*>(m_pPrimitive);
    return primitive->TryLock();
}

void CMutex::Unlock()
{
    if (!m_pPrimitive)
    {
        assert(1 == 0);
    }
    CMutexPrimitive * primitive = static_cast<CMutexPrimitive*>(m_pPrimitive);
    return primitive->Unlock();
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