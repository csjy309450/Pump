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

//������ (MAX_THREADPOOL_SIZE) ���̳߳�
#define MAX_THREADPOOL_SIZE 64
//ÿ���̳߳����(MAX_THREAD_PER_THREADPOOL)���߳�
#define MAX_THREAD_PER_THREADPOOL 512

struct __PUMP_THREAD_INFO
{
    pump_uint32_t nWorkIndex;  //���������е�λ��
    pump_pvoid_t pThreadPool;

    pump_bool_t bIdle;         //�߳��Ƿ���е���˼, TRUE�������, FALSE����������
    pump_bool_t bQuit;         //�߳��Ƿ��˳�
    pump_handle_t hThreadId;   //�߳�ID, ͬʱ��������ж��߳��Ƿ񴴽�

    pump_sem_t hSem; //����˯���߳�

    PUMP_CORE_ThreadPool_WorkRoutine fWork;//ִ���̻߳ص�����
    pump_pvoid_t pParam;
};

struct __PUMP_THREAD_POOL_T
{
    pump_bool_t bUsed;

    pump_bool_t bQuit;         // �̳߳���Ҫ�˳�,��֤����PUMP_CORE_ThreadPool_Destroy��PUMP_CORE_ThreadPool_WorkExͬʱִ��ʱ�߼��ϵ�����
    pump_uint32_t nMaxCount;
    pump_uint32_t nInitCount;
    pump_uint32_t nCurrCount;  // ֮ǰ�̲߳��������˳�,���Դ����̵߳�struWorks�����0��nCurrCount
    // �������ģ�����ֻ�Ǵ��������̸߳���
    pump_uint32_t nStackSize;
    pump_uint32_t nTimeOut;    // ms, ���ָ��ʱ���߳�û������,���Զ��˳�

    pump_mutex_t hLock;

    __PUMP_THREAD_INFO struWorks[MAX_THREAD_PER_THREADPOOL];
};

//////////////////////////////////////////////////////////////////////////////
static __PUMP_THREAD_POOL_T s_struThreadPools[MAX_THREADPOOL_SIZE];
static pump_mutex_t s_csLock;

//��������̳߳�
static __PUMP_THREAD_POOL_T* PUMP_CORE_GetIdleThreadPool_Local();
static pump_void_t PUMP_CORE_MoveToIdleThreadPool_Local(__PUMP_THREAD_POOL_T* pThreadPool);

//��Ե����̳߳�
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
 *������Ҫ���ϲ㱣��ԭ���Ƕ�̬�����̵߳�ʱ��Ҳ��Ҫ�������.
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

        // ��ʱbIdle����ΪFALSE��������������������߳̾Ͳ��ᱻ��������
        // �Ӷ������˳���
        pThreadInfo->bIdle = PUMP_FALSE;
        pThreadInfo->bQuit = PUMP_TRUE;
        PUMP_CORE_SemPost(&pThreadInfo->hSem);

        PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

        //���������ڲ���ԭ�����߳��ڷ�����ж���ʱ��Ҫ�����,���������.
        PUMP_CORE_Thread_Wait(pThreadInfo->hThreadId);
        // ������仰������������,��Ϊ����̵߳Ĵ���δ��ȫ�˳�,�������WorkIndex�ͱ������ȥ��
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
        // PUMP_CORE_CreateThread_Local�ڲ�û����
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
            // PUMP_CORE_CreateThread_Local�ڲ�����
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
            // �̴߳����ҿ���
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

        // ǰnInitCount���̲߳����˳�
        if (pThreadPool->nTimeOut == PUMP_INFINITE || pThreadInfo->nWorkIndex < pThreadPool->nInitCount)
        {
            ret = PUMP_CORE_SemWait(&pThreadInfo->hSem);
            if (ret == PUMP_ERROR)
            {
                // �ź����쳣�����п������յ��ж��źţ�����������������ʱ�᷵��-1
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
                    // ��Ϊ���̳߳�ȫ�������,����ִ����δ���ʱ, ���ô����̲߳���ͬʱִ��
                    // PUMP_CORE_DestroyThread_Local�ͷ��̺߳���ʱ�ᷢ���ź���,���Բ����ܽ��뵽������
                    PUMP_CORE_SemDestroy(&pThreadInfo->hSem);
                    memset(pThreadInfo, 0x0, sizeof(__PUMP_THREAD_INFO));
                    pThreadInfo->hThreadId = (pump_handle_t)PUMP_INVALID_THREAD;

                    pThreadPool->nCurrCount--;
                    PUMP_CORE_MutexUnlock(&pThreadPool->hLock);

                    // ���е������, ����߳̾��Ѿ����̳߳����κι�ϵ��
                    // ���WorkIndex���������´������߳���
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
    //Ĭ�Ͻӿ�ָ��Ĭ�϶�ջ��С0 == 1M
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
    //2010-08-26 �����̶߳�ջ���������ϲ�ָ���̶߳�ջ��С
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
    //��������6��24���޸ĵ��߼����޿����߳�ʱ�����ȴ�ֱ�ӷ���ʧ��
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
            //��̬�����߳���
            if (pThreadPool->nCurrCount < pThreadPool->nMaxCount)
            {
                pump_int32_t ret = PUMP_CORE_CreateThread_Local(pThreadPool);
                if (ret == PUMP_OK)
                {
                    // ���������߳̾ͱ�������Ͷ������, ��ֹ�߳�������δͶ��ǰ���˳�
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
                //2010-08-26 �����ķ���ʧ����������ģ��������ӱ�ǹ��ϲ�ѡ��ȴ����
                //schina�޸�bug:�̴߳ﵽ���ֵʱһֱ�ڻ�ȡ�����̣߳�����CPU�ܸߵ�����
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
�������� : Create
�������� : �����̳߳�
�������˵�� : InitThreadNumΪ��ʼ���̸߳�����MaxThreadNumΪ����̸߳���
��������ֵ��˵�� : �ɹ�����PUMP_OK��ʧ�ܷ���PUMP_ERROR
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
�������� : Destroy
�������� : �����̳߳�
�������˵�� : ��
��������ֵ��˵�� : �ɹ�����PUMP_OK��ʧ�ܷ���PUMP_ERROR
*************************************************/
pump_int32_t CThreadPool::Destroy(pump_void_t)
{
    return PUMP_CORE_ThreadPool_Destroy(m_threadpoolhandle);
}

/*************************************************
�������� : Work
�������� : ѹ�빤������
�������˵�� : WorkRoutineΪ��������
��������ֵ��˵�� : �ɹ�����PUMP_OK��ʧ�ܷ���PUMP_ERROR
*************************************************/
pump_int32_t CThreadPool::Work(pump_pvoid_t(PUMP_CALLBACK *WorkRoutine)(pump_pvoid_t), pump_pvoid_t Params)
{
    return PUMP_CORE_ThreadPool_Work(m_threadpoolhandle, WorkRoutine, Params);
}

}
}
}

#endif // (defined PUMP_COMPILER_CXX)