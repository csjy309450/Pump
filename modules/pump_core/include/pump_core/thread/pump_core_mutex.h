/**
 * @file pump_core_mutex.h
 * @brief Library Inner Header.
 * @details Declaration of mutex control interface.
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

#ifndef PUMP_CORE_MUTEX_H
#define PUMP_CORE_MUTEX_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/pump_core_object.h"

#if defined(PUMP_OS_WINDOWS)
typedef CRITICAL_SECTION pump_mutex_t;
#define PUMP_MUTEX_FAST			0
#define PUMP_MUTEX_RECURSIVE		0
#define PUMP_MUTEX_ERRORCHECK	0
#elif defined(PUMP_OS_POSIX)
#include <pthread.h>
typedef pthread_mutex_t pump_mutex_t;

#   if (defined(PUMP_OS_APPLE) || defined(PUMP_OS_S60) || defined(PUMP_OS_ANDROID))
#define PUMP_MUTEX_FAST                   PTHREAD_MUTEX_NORMAL
#define PUMP_MUTEX_RECURSIVE		    PTHREAD_MUTEX_RECURSIVE
#define PUMP_MUTEX_ERRORCHECK    PTHREAD_MUTEX_ERRORCHECK
#   elif defined (PUMP_OS_LINUX)
#define PUMP_MUTEX_FAST			        PTHREAD_MUTEX_FAST_NP
#define PUMP_MUTEX_RECURSIVE		    PTHREAD_MUTEX_RECURSIVE_NP
#define PUMP_MUTEX_ERRORCHECK   PTHREAD_MUTEX_ERRORCHECK_NP
#   endif
#else
#error OS Not Implement yet.
#endif // defined(PUMP_OS_WINDOWS)

/**
* PUMP_CORE_MutexCreate create mutex
* @param pMutex (OUT) mutex to create
* @param iFlag (IN) mutex flag.
* @return 0 success, -1 fail.
* @sa PUMP_CORE_MutexDestroy
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_MutexCreate(pump_mutex_t* pMutex, pump_int32_t iFlag = -1);

/**
* PUMP_CORE_MutexDestroy destroy mutex created by PUMP_CORE_MutexCreate.
* @param pMutex (IN) mutex created by PUMP_CORE_MutexCreate.
* @return 0 success, -1 fail.
* @sa PUMP_CORE_MutexCreate
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_MutexDestroy(pump_mutex_t* pMutex);

/**
* PUMP_CORE_MutexLock lock mutex
* @param pMutex (IN) mutex created by PUMP_CORE_MutexCreate.
* @return 0 success, -1 fail.
* @sa PUMP_CORE_MutexCreate,PUMP_CORE_MutexTryLock(),PUMP_CORE_MutexUnlock().
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_MutexLock(pump_mutex_t* pMutex);

/**
* PUMP_CORE_MutexTryLock try lock mutex
* @param pMutex (IN) mutex created by PUMP_CORE_MutexCreate.
* @return 0 success, -1 fail.
* @sa PUMP_CORE_MutexCreate(),PUMP_CORE_MutexUnlock(),PUMP_CORE_MutexLock().
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_MutexTryLock(pump_mutex_t* pMutex);

/**
* PUMP_CORE_MutexUnlock unlock mutex
* @param pMutex (IN) mutex created by PUMP_CORE_MutexCreate.
* @return 0 success, -1 fail.
* @sa PUMP_CORE_MutexCreate(),PUMP_CORE_MutexTryLock(),PUMP_CORE_MutexLock()
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_MutexUnlock(pump_mutex_t* pMutex);

#ifdef PUMP_COMPILER_CXX
namespace Pump
{
namespace Core
{
namespace Thread
{

class PUMP_CORE_CLASS CMutex
    : public CObjectBase
{
public:
    CMutex();
    virtual ~CMutex();
    void Lock();
    bool TryLock();
    void Unlock();
private:
    CMutex(const CMutex & other);
};

class PUMP_CORE_CLASS CMutexGuilder
{
public:
    CMutexGuilder(CMutex & locker);
    ~CMutexGuilder();
private:
    CMutexGuilder();
    CMutexGuilder(const CMutexGuilder &);
private:
    CMutex & m_locker;
};

PUMP_CORE_EXP void PUMP_CALLBACK MutexLocker(CMutex &mux0, CMutex &mux1);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexLocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5, CMutex &mux6);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5);

PUMP_CORE_EXP void PUMP_CALLBACK  MutexUnlocker(CMutex &mux0, CMutex &mux1, CMutex &mux2, CMutex &mux3, CMutex &mux4, CMutex &mux5, CMutex &mux6);

}
}
}
#endif // PUMP_COMPILER_CXX

#endif // PUMP_CORE_MUTEX_H
