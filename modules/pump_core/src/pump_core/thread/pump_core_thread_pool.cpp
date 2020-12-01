/**
 * @file pump_core_thread_pool.cpp
 * @brief Library Implementation.
 * @details Implementation of thread pool control interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-12-01
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-12-01<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/thread/pump_core_semaphore.h"
#include "pump_core/thread/pump_core_thread.h"
#include "pump_core/thread/pump_core_thread_pool.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/time/pump_core_time.h"

//最多可有 (MAX_THREADPOOL_SIZE) 个线程池
#define MAX_THREADPOOL_SIZE 64
//每个线程池最多(MAX_THREAD_PER_THREADPOOL)个线程
#define MAX_THREAD_PER_THREADPOOL 512

struct __PUMP_THREAD_INFO
{
    pump_uint32_t nWorkIndex;  //工作队列中的位置
    pump_pvoid_t pThreadPool;

    pump_bool_t bIdle;         //线程是否空闲的意思, TRUE代表空闲, FALSE代表有任务
    pump_bool_t bQuit;         //线程是否退出
    pump_handle_t hThreadId;   //线程ID, 同时用这个来判断线程是否创建

    pump_sem_t hSem; //唤醒睡眠线程

    PUMP_CORE_ThreadPool_WorkRoutine fWork;//执行线程回调函数
    pump_pvoid_t pParam;
};

struct __PUMP_THREAD_POOL_T
{
    pump_bool_t bUsed;

    pump_bool_t bQuit;         // 线程池需要退出,保证函数PUMP_CORE_ThreadPool_Destroy、PUMP_CORE_ThreadPool_WorkEx同时执行时逻辑上的问题
    pump_uint32_t nMaxCount;
    pump_uint32_t nInitCount;
    pump_uint32_t nCurrCount;  // 之前线程不会主动退出,所以代表线程的struWorks数组从0到nCurrCount
    // 是连续的，现在只是代表创建的线程个数
    pump_uint32_t nStackSize;
    pump_uint32_t nTimeOut;    // ms, 如果指定时间线程没有运行,会自动退出

    pump_mutex_t hLock;

    __PUMP_THREAD_INFO struWorks[MAX_THREAD_PER_THREADPOOL];
};

//////////////////////////////////////////////////////////////////////////////
static __PUMP_THREAD_POOL_T s_struThreadPools[MAX_THREADPOOL_SIZE];
static pump_mutex_t s_csLock;

//针对整个线程池
static __PUMP_THREAD_POOL_T* PUMP_CORE_GetIdleThreadPool_Local();
static pump_void_t PUMP_CORE_MoveToIdleThreadPool_Local(__PUMP_THREAD_POOL_T* pThreadPool);

//针对单个线程池
static pump_int32_t PUMP_CORE_InitThread_Local(__PUMP_THREAD_POOL_T* pThreadPool);
static pump_void_t PUMP_CORE_FiniThread_Local(__PUMP_THREAD_POOL_T* pThreadPool);

static pump_int32_t PUMP_CORE_CreateThread_Local(__PUMP_THREAD_POOL_T* pThreadPool);
static pump_int32_t PUMP_CORE_DestroyThread_Local(__PUMP_THREAD_POOL_T* pThreadPool);

static pump_int32_t PUMP_CORE_MoveToIdle_Local(__PUMP_THREAD_POOL_T* pThreadPool, __PUMP_THREAD_INFO* pThreadInfo);

static __PUMP_THREAD_INFO* PUMP_CORE_GetIdle_Local(__PUMP_THREAD_POOL_T* pThreadPool);

static pump_pvoid_t PUMP_CALLBACK f_Thread_Svc(pump_pvoid_t);

//////////////////////////////////////////////////////////////////////////////
pump_int32_t PUMP_CORE_InitThreadPool()
{
    if (PUMP_ERROR == PUMP_CORE_MutexCreate(&s_csLock, PUMP_MUTEX_FAST))
    {
        return PUMP_ERROR;
    }
    memset(s_struThreadPools, 0, (sizeof(__PUMP_THREAD_POOL_T)*MAX_THREADPOOL_SIZE));

    for (int index = 0; index < MAX_THREADPOOL_SIZE; index++)
    {
        for (int cnt = 0; cnt < MAX_THREAD_PER_THREADPOOL; cnt++)
        {
            s_struThreadPools[index].struWorks[cnt].hThreadId = (pump_handle_t)PUMP_INVALID_THREAD;
        }
    }

    return PUMP_OK;
}

pump_void_t PUMP_CORE_FiniThreadPool()
{
    PUMP_CORE_MutexDestroy(&s_csLock);
}

/////////////////////////////////////////////////////////////////////////////
static __PUMP_THREAD_POOL_T* PUMP_CORE_GetIdleThreadPool_Local()
{
    pump_int32_t i = 0;
    __PUMP_THREAD_POOL_T* pThreadPool = NULL;

    PUMP_CORE_MutexLock(&s_csLock);

    for (; i<MAX_THREADPOOL_SIZE; i++)
    {
        if (!s_struThreadPools[i].bUsed)
        {
            s_struThreadPools[i].bUsed = PUMP_TRUE;
            pThreadPool = &s_struThreadPools[i];
            break;
        }
    }

    PUMP_CORE_MutexUnlock(&s_csLock);

    return pThreadPool;
}

static pump_void_t PUMP_CORE_MoveToIdleThreadPool_Local(__PUMP_THREAD_POOL_T* pThreadPool)
{
    PUMP_CORE_MutexLock(&s_csLock);
    pThreadPool->bUsed = PUMP_FALSE;
    PUMP_CORE_MutexUnlock(&s_csLock);

    return;
}

//////////////////////////////////////////////////////////////////
//
/**
 *互斥需要在上层保护原因是动态增加线程的时候也需要这个函数.
 */
static pump_int32_t PUMP_CORE_CreateThread_Local(__PUMP_THREAD_POOL_T* pThreadPool)
{
    pump_handle_t hThreadId = (pump_handle_t)PUMP_INVALID_THREAD;
    __PUMP_THREAD_INFO* pThreadInfo = NULL;

    if (pThreadPool->nCurrCount == pThreadPool->nMaxCount)
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_CreateThread_Local (pThreadPool->nCurrCount == pThreadPool->nMaxCount) error, return -1, 1\n");
        return PUMP_ERROR;
    }

    for (pump_uint32_t index = 0; index < pThreadPool->nMaxCount; index++)
    {
        pThreadInfo = &(pThreadPool->struWorks[index]);
        if (pThreadInfo->hThreadId != (pump_handle_t)PUMP_INVALID_THREAD)
        {
            continue;
        }

        pThreadInfo->fWork = NULL;
        if (PUMP_ERROR == PUMP_CORE_SemCreate(&pThreadInfo->hSem, 0))
        {
            //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_CreateThread_Local PUMP_CORE_SemCreate error, return -1 2\n");
            return PUMP_ERROR;
        }

        pThreadInfo->bQuit = PUMP_FALSE;
        pThreadInfo->pThreadPool = pThreadPool;
        pThreadInfo->nWorkIndex = index;

        pump_int32_t nStackSize = pThreadPool->nStackSize == 0 ? (1024 * 1024) : pThreadPool->nStackSize;
        hThreadId = PUMP_CORE_Thread_Create(f_Thread_Svc, pThreadInfo, nStackSize);
        if (hThreadId == (pump_handle_t)PUMP_INVALID_THREAD)
        {
            //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_CreateThread_Local PUMP_CORE_Thread_Create error, return -1 3\n");

            PUMP_CORE_SemDestroy(&pThreadInfo->hSem);
            memset(pThreadInfo, 0x0, sizeof(__PUMP_THREAD_INFO));
            pThreadInfo->hThreadId = (pump_handle_t)PUMP_INVALID_THREAD;

            return PUMP_ERROR;
        }

        pThreadInfo->bIdle = PUMP_TRUE;
        pThreadInfo->hThreadId = hThreadId;

        pThreadPool->nCurrCount++;
        return PUMP_OK;
    }

    return PUMP_ERROR;
}

static pump_int32_t PUMP_CORE_DestroyThread_Local(__PUMP_THREAD_POOL_T* pThreadPool)
{
    __PUMP_THREAD_INFO* pThreadInfo = NULL;

    PUMP_CORE_MutexLock(&pThreadPool->hLock);

    if (pThreadPool->nCurrCount == 0)
    {
        PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
        return PUMP_ERROR;
    }

    for (pump_uint32_t index = 0; index < pThreadPool->nMaxCount; index++)
    {
        pThreadInfo = &(pThreadPool->struWorks[index]);
        if (pThreadInfo->hThreadId == (pump_handle_t)PUMP_INVALID_THREAD)
        {
            continue;
        }

        // 此时bIdle必须为FALSE，代表有任务，这样这个线程就不会被分配任务
        // 从而可以退出了
        pThreadInfo->bIdle = PUMP_FALSE;
        pThreadInfo->bQuit = PUMP_TRUE;
        PUMP_CORE_SemPost(&pThreadInfo->hSem);

        PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

        //不放在锁内部的原因是线程在放入空闲队列时需要这个锁,会造成死锁.
        PUMP_CORE_Thread_Wait(pThreadInfo->hThreadId);
        // 下面这句话必须放在锁里边,因为这个线程的处理还未完全退出,否则这个WorkIndex就被分配出去了
        // pThreadInfo->hThreadId = PUMP_INVALID_THREAD; 

        PUMP_CORE_MutexLock(&pThreadPool->hLock);

        PUMP_CORE_SemDestroy(&pThreadInfo->hSem);
        memset(pThreadInfo, 0x0, sizeof(__PUMP_THREAD_INFO));
        pThreadInfo->hThreadId = (pump_handle_t)PUMP_INVALID_THREAD;
        pThreadPool->nCurrCount--;

        break;
    }

    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
    return PUMP_OK;
}

static pump_int32_t PUMP_CORE_InitThread_Local(__PUMP_THREAD_POOL_T* pThreadPool)
{
    pump_int32_t iRet = PUMP_OK;
    if (pThreadPool == NULL)
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_InitThread_Local pThreadPool == NULL error, return -1 1\n");
        return PUMP_ERROR;
    }

    PUMP_CORE_MutexCreate(&pThreadPool->hLock);

    PUMP_CORE_MutexLock(&pThreadPool->hLock);
    for (pump_uint32_t i = 0; i<pThreadPool->nInitCount; i++)
    {
        // PUMP_CORE_CreateThread_Local内部没有锁
        if (PUMP_ERROR == PUMP_CORE_CreateThread_Local(pThreadPool))
        {
            //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_InitThread_Local PUMP_CORE_CreateThread_Local error, return -1 2\n");
            iRet = PUMP_ERROR;

            break;
        }
    }
    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

    if (iRet == PUMP_ERROR)
    {
        for (pump_uint32_t i = 0; i < pThreadPool->nMaxCount; i++)
        {
            // PUMP_CORE_CreateThread_Local内部有锁
            PUMP_CORE_DestroyThread_Local(pThreadPool);
        }
    }

    return iRet;
}

static pump_void_t PUMP_CORE_FiniThread_Local(__PUMP_THREAD_POOL_T* pThreadPool)
{
    pThreadPool->bQuit = PUMP_TRUE;
    for (pump_uint32_t i = 0; i < pThreadPool->nMaxCount; i++)
    {
        PUMP_CORE_DestroyThread_Local(pThreadPool);
    }

    PUMP_CORE_MutexDestroy(&pThreadPool->hLock);

    return;
}

static pump_int32_t PUMP_CORE_MoveToIdle_Local(__PUMP_THREAD_POOL_T* pThreadPool, __PUMP_THREAD_INFO* pThreadInfo)
{
    pump_int32_t i = 0;

    PUMP_CORE_MutexLock(&pThreadPool->hLock);

    i = pThreadInfo->nWorkIndex;
    pThreadPool->struWorks[i].bIdle = PUMP_TRUE;

    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

    return PUMP_OK;
}

static __PUMP_THREAD_INFO* PUMP_CORE_GetIdle_Local(__PUMP_THREAD_POOL_T* pThreadPool)
{
    pump_uint32_t i = 0;
    __PUMP_THREAD_INFO* pTmpThreadInfo = NULL;

    if ((pThreadPool == NULL))
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_GetIdle_Local pThreadPool == NULL error, return NULL 1\n");
        return NULL;
    }

    PUMP_CORE_MutexLock(&pThreadPool->hLock);

    for (; i<pThreadPool->nMaxCount; i++)
    {
        if ((pThreadPool->struWorks[i].bIdle == PUMP_TRUE) &&
            (pThreadPool->struWorks[i].hThreadId != (pump_handle_t)PUMP_INVALID_THREAD))
        {
            // 线程存在且空闲
            pTmpThreadInfo = &pThreadPool->struWorks[i];
            pThreadPool->struWorks[i].bIdle = PUMP_FALSE;
            break;
        }
    }

    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

    return pTmpThreadInfo;
}

static pump_pvoid_t PUMP_CALLBACK f_Thread_Svc(pump_pvoid_t pParam)
{
    __PUMP_THREAD_INFO* pThreadInfo = NULL;
    if (pParam == NULL)
    {
        //PUMP_CORE_OutputDebug("schina !!! f_Thread_Svc pParam == NULL error, return NULL 1\n");
        return NULL;
    }

    pThreadInfo = (__PUMP_THREAD_INFO*)(pParam);
    pump_int32_t ret = PUMP_OK;

    for (;;)
    {
        __PUMP_THREAD_POOL_T* pThreadPool = (__PUMP_THREAD_POOL_T*)(pThreadInfo->pThreadPool);

        // 前nInitCount个线程不会退出
        if (pThreadPool->nTimeOut == PUMP_INFINITE || pThreadInfo->nWorkIndex < pThreadPool->nInitCount)
        {
            ret = PUMP_CORE_SemWait(&pThreadInfo->hSem);
            if (ret == PUMP_ERROR)
            {
                // 信号量异常返回有可能是收到中断信号，并非真正的有数据时会返回-1
                continue;
            }
        }
        else
        {
            ret = PUMP_CORE_SemTimedWait(&pThreadInfo->hSem, pThreadPool->nTimeOut);
            if (ret == PUMP_ERROR)
            {
                pump_handle_t hThreadId = pThreadInfo->hThreadId;
                PUMP_CORE_MutexLock(&pThreadPool->hLock);
                ret = PUMP_CORE_SemTimedWait(&pThreadInfo->hSem, 0);
                if (ret == PUMP_ERROR)
                {
                    // 因为在线程池全局锁里边,所以执行这段代码时, 调用创建线程不能同时执行
                    // PUMP_CORE_DestroyThread_Local释放线程函数时会发送信号量,所以不可能进入到这个里边
                    PUMP_CORE_SemDestroy(&pThreadInfo->hSem);
                    memset(pThreadInfo, 0x0, sizeof(__PUMP_THREAD_INFO));
                    pThreadInfo->hThreadId = (pump_handle_t)PUMP_INVALID_THREAD;

                    pThreadPool->nCurrCount--;
                    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

                    // 运行到这里后, 这个线程就已经和线程池无任何关系了
                    // 这个WorkIndex可以留给新创建的线程了
#ifdef PUMP_OS_WINDOWS
                    CloseHandle(hThreadId);
#elif defined(PUMP_OS_POSIX)
                    pthread_detach((pthread_t)hThreadId);
#else
#error os not supported.
#endif
                    return NULL;
                }

                PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
            }
        }

        if (pThreadInfo->bQuit)
        {
            break;
        }
        else
        {
            if (pThreadInfo->fWork)
            {
                pThreadInfo->fWork(pThreadInfo->pParam);
                pThreadInfo->fWork = NULL;
                pThreadInfo->pParam = NULL;
            }

            PUMP_CORE_MoveToIdle_Local((__PUMP_THREAD_POOL_T*)(pThreadInfo->pThreadPool), pThreadInfo);
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadPool_Create(pump_uint32_t nInitCount, pump_uint32_t nMaxCount)
{
    //默认接口指定默认堆栈大小0 == 1M
    return PUMP_CORE_ThreadPool_CreateFlex(nInitCount, nMaxCount, 0, PUMP_INFINITE);
}

PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadPool_CreateEx(pump_uint32_t nInitCount, pump_uint32_t nMaxCount, pump_uint32_t nStackSize)
{
    return PUMP_CORE_ThreadPool_CreateFlex(nInitCount, nMaxCount, nStackSize, PUMP_INFINITE);
}

PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadPool_CreateFlex(pump_uint32_t nInitCount, pump_uint32_t nMaxCount,
    pump_uint32_t nStackSize, pump_uint32_t nTimeOut)
{
    __PUMP_THREAD_POOL_T* pThreadPool = NULL;

    if (nMaxCount > MAX_THREAD_PER_THREADPOOL)
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_ThreadPool_Create nMaxCount > MAX_THREAD_PER_THREADPOOL error, return -1 1\n");
        return NULL;
    }

    pThreadPool = PUMP_CORE_GetIdleThreadPool_Local();
    if (pThreadPool == NULL)
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_ThreadPool_Create PUMP_CORE_GetIdleThreadPool_Local error, return -1 2\n");
        return (pump_handle_t)(NULL);
    }

    pThreadPool->nInitCount = nInitCount;
    pThreadPool->nMaxCount = nMaxCount;
    pThreadPool->nCurrCount = 0;
    //2010-08-26 增加线程堆栈参数用于上层指定线程堆栈大小
    pThreadPool->nStackSize = nStackSize;
    pThreadPool->nTimeOut = nTimeOut;

    if (PUMP_CORE_InitThread_Local(pThreadPool) == 0)
    {
        pThreadPool->bQuit = PUMP_FALSE;
        return pThreadPool;
    }
    else
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_ThreadPool_Create PUMP_CORE_InitThread_Local error, return -1 3\n");
        PUMP_CORE_MoveToIdleThreadPool_Local(pThreadPool);
        //pThreadPool = NULL;
    }

    return NULL;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadPool_Destroy(pump_handle_t hHandle)
{
    if (hHandle == NULL)
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_ThreadPool_Destroy pThreadPool == NULL return\n");
        return PUMP_ERROR;
    }

    PUMP_CORE_FiniThread_Local((__PUMP_THREAD_POOL_T*)(hHandle));
    PUMP_CORE_MoveToIdleThreadPool_Local((__PUMP_THREAD_POOL_T*)(hHandle));

    return PUMP_OK;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadPool_Work(pump_handle_t hHandle, PUMP_CORE_ThreadPool_WorkRoutine pWork, void* pParam)
{
    //这里沿用6月24日修改的逻辑，无空闲线程时不做等待直接返回失败
    return PUMP_CORE_ThreadPool_WorkEx(hHandle, pWork, pParam, PUMP_FALSE);
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadPool_WorkEx(pump_handle_t hHandle, PUMP_CORE_ThreadPool_WorkRoutine pWork, void* pParam, pump_bool_t bWaitForIdle)
{
    __PUMP_THREAD_INFO* pThreadInfo = NULL;
    __PUMP_THREAD_POOL_T* pThreadPool = NULL;

    if ((hHandle == NULL) || (pWork == NULL))
    {
        //PUMP_CORE_OutputDebug("schina !!! PUMP_CORE_ThreadPool_Work param error, return -1 1\n");
        return PUMP_ERROR;
    }

    pThreadPool = (__PUMP_THREAD_POOL_T*)(hHandle);

    while (pThreadPool->bQuit == PUMP_FALSE)
    {
        PUMP_CORE_MutexLock(&pThreadPool->hLock);
        pThreadInfo = PUMP_CORE_GetIdle_Local(pThreadPool);
        if (pThreadInfo == NULL)
        {
            //动态增加线程数
            if (pThreadPool->nCurrCount < pThreadPool->nMaxCount)
            {
                pump_int32_t ret = PUMP_CORE_CreateThread_Local(pThreadPool);
                if (ret == PUMP_OK)
                {
                    // 当创建了线程就必须立刻投递任务, 防止线程在任务未投递前就退出
                    pThreadInfo = PUMP_CORE_GetIdle_Local(pThreadPool);
                    pThreadInfo->fWork = pWork;
                    pThreadInfo->pParam = pParam;
                    PUMP_CORE_SemPost(&pThreadInfo->hSem);

                    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
                    return PUMP_OK;
                }
                else
                {
                    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
                    return PUMP_ERROR;
                }
            }
            else if (!bWaitForIdle)
            {
                //2010-08-26 单纯的返回失败是有问题的，因此这里加标记供上层选择等待与否
                //schina修改bug:线程达到最大值时一直在获取空余线程，导致CPU很高的问题
                PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
                return PUMP_ERROR;
            }
            else
            {
                PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
                PUMP_CORE_Sleep(5);
                continue;
            }
        }
        else
        {
            pThreadInfo->fWork = pWork;
            pThreadInfo->pParam = pParam;
            PUMP_CORE_SemPost(&pThreadInfo->hSem);
        }

        PUMP_CORE_MutexUnlock(&pThreadPool->hLock);
        return PUMP_OK;
    }

    return PUMP_ERROR;
}

#if (defined PUMP_COMPILER_CXX)
namespace Pump
{
namespace Core
{
namespace Thread
{
CThreadPool::CThreadPool(pump_void_t)
    : m_threadpoolhandle(PUMP_INVALID_HANDLE)
{
}

CThreadPool::~CThreadPool(pump_void_t)
{
    m_threadpoolhandle = (pump_handle_t)PUMP_INVALID_THREAD;
}

/*************************************************
函数名称 : Create
函数功能 : 创建线程池
输入参数说明 : InitThreadNum为初始化线程个数；MaxThreadNum为最大线程个数
函数返回值的说明 : 成功返回PUMP_OK，失败返回PUMP_ERROR
*************************************************/
pump_int32_t CThreadPool::Create(pump_uint32_t InitThreadNum, pump_uint32_t MaxThreadNum)
{
    m_threadpoolhandle = PUMP_CORE_ThreadPool_Create(InitThreadNum, MaxThreadNum);
    if (m_threadpoolhandle == PUMP_INVALID_HANDLE)
    {
        return PUMP_ERROR;
    }
    else
    {
        return PUMP_OK;
    }
}

/*************************************************
函数名称 : Destroy
函数功能 : 销毁线程池
输入参数说明 : 无
函数返回值的说明 : 成功返回PUMP_OK，失败返回PUMP_ERROR
*************************************************/
pump_int32_t CThreadPool::Destroy(pump_void_t)
{
    return PUMP_CORE_ThreadPool_Destroy(m_threadpoolhandle);
}

/*************************************************
函数名称 : Work
函数功能 : 压入工作例程
输入参数说明 : WorkRoutine为工作例程
函数返回值的说明 : 成功返回PUMP_OK，失败返回PUMP_ERROR
*************************************************/
pump_int32_t CThreadPool::Work(pump_pvoid_t(PUMP_CALLBACK *WorkRoutine)(pump_pvoid_t), pump_pvoid_t Params)
{
    return PUMP_CORE_ThreadPool_Work(m_threadpoolhandle, WorkRoutine, Params);
}

}
}
}

#endif // (defined PUMP_COMPILER_CXX)