/**
 * @file pump_core_semaphore.cpp
 * @brief Library Implementation.
 * @details Implementation of semaphore interface.
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

#include "pump_macro/pump_pre.h"
#ifdef PUMP_OS_POSIX
#include <sys/time.h>
#endif // PUMP_OS_POSIX
#include "pump_core/thread/pump_core_semaphore.h"

#if (defined PUMP_OS_APPLE)
//#include "TimedWait.h"
static int iSemCount = 0;
extern pump_mutex_t s_hCreateLock;

#define SEM_NAME_LEN            64
//���ڹر��ź���ʱ��Ҫɾ�����ƣ�������Ҫ��¼�ź�������
typedef struct
{
    pump_sem_t semReal;    //sem_t* Ҫ���ڵ�һ��λ��
    char semName[SEM_NAME_LEN];
}__PUMP_SEM_T_MAC;

#define __PUMP_NAME_IN_GLOBALSEM(sem)    ((*(__PUMP_SEM_T_MAC*)(sem)).semName)
#define __PUMP_SEM_IN_GLOBALSEM(sem)    ((*(__PUMP_SEM_T_MAC*)(sem)).semReal)
#endif // (defined PUMP_OS_APPLE)

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemCreate(pump_sem_t* pSem, pump_uint32_t nInitCount/*, pump_uint32_t nMaxCount*/)
{
#if (defined PUMP_OS_WINDOWS)
#   if defined PUMP_OS_WINPHONE
    HANDLE hHandle = CreateSemaphoreExW(NULL, nInitCount, 65535, NULL, 0, SYNCHRONIZE | SEMAPHORE_MODIFY_STATE);
#   else
    HANDLE hHandle = CreateSemaphore(NULL, nInitCount, 65535, NULL);
#   endif
    if (hHandle == NULL)
    {
        return PUMP_ERROR;
    }

    *pSem = hHandle;

    return PUMP_OK;
#elif (defined PUMP_OS_POSIX)
    if ( !pSem )
    {
        return PUMP_ERROR;
    }
#   if defined (PUMP_OS_APPLE)
    char szCount[SEM_NAME_LEN] = { 0 };
    int iTmpSum = 0; //make sure that no more than SEM_VALUE_MAX cycle times was made

    //2011-08-15 sem_open�����ź���ʱ����O_EXCL��ǣ��Ա��ź��������Ѵ���ʱ�ܹ�����ʧ��
    //��������ѭ�����Եķ�ʽ�����ź������ֵΪ���ޣ��ɹ�����Ѵ���ʧ�ܾ�ֱ�ӷ���
    while (iTmpSum < SEM_VALUE_MAX)
    {
        PUMP_CORE_MutexLock(&s_hCreateLock);
        sprintf(szCount, "hpr_named_semaphore_%05d", iSemCount);
        //����ѭ����û�б�Ҫ�������������
        ++iSemCount %= SEM_VALUE_MAX;
        ++iTmpSum;
        PUMP_CORE_MutexUnlock(&s_hCreateLock);

        sem_t* pTmpSem = sem_open(szCount, O_CREAT | O_EXCL, 0777, nInitCount);
        if (pTmpSem == SEM_FAILED)
        {
            if (errno == EEXIST)
            {
                //fprintf(stderr, "sem_open[%s] EEXIST occurred\n", szCount);
                //2011-08-15 ����������ź����Ѵ��ڣ������ѭ������������ʧ����
                continue;
            }
            else
            {
                return PUMP_ERROR;
            }
        }
        else
        {
            __PUMP_SEM_T_MAC* tmp_sem = (__PUMP_SEM_T_MAC *)malloc(sizeof(__PUMP_SEM_T_MAC));
            if (tmp_sem == NULL)
            {
                sem_unlink(szCount);
                sem_close(pTmpSem);
                return PUMP_ERROR;
            }

            tmp_sem->semReal = pTmpSem;
            strncpy(tmp_sem->semName, szCount, sizeof(tmp_sem->semName));
            //2011-08-15 ������ǿ������ת�������ڲ��ṹ����רΪ�ⲿ����
            //����ڲ��ṹ��sem_t*һ��Ҫ���ڵ�һ��λ��
            //���ﲻ�ص�������λ�����⣬macosx��pump_sem_tҲ��ָ������
            *pSem = (pump_sem_t)tmp_sem;
            //fprintf(stderr, "sem_open %p with name[%s] addr[%p]\n", pTmpSem, szCount, *pSem);

            while (1)
            {
                if (-1 == sem_trywait(pTmpSem))
                {
                    for (int i = 0; i < nInitCount; i++)
                    {
                        sem_post(pTmpSem);
                    }
                    break;
                }
            }
            return PUMP_OK;
        }
    }
    return PUMP_ERROR;
#   else
    return sem_init(pSem, 0, nInitCount);
#   endif
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemDestroy(pump_sem_t* pSem)
{
#if (defined PUMP_OS_WINDOWS)
    if (!pSem)
    {
        return PUMP_ERROR;
    }
    return (CloseHandle(*pSem)) ? PUMP_OK : PUMP_ERROR;
#elif (defined PUMP_OS_POSIX)
    if (!pSem)
    {
        return PUMP_ERROR;
    }

#   if defined (PUMP_OS_APPLE)
    __PUMP_SEM_T_MAC *tmp_sem = (__PUMP_SEM_T_MAC *)(*pSem);
    //2011-08-15  �ر��ź���֮ǰ��ע�����ƣ�Ȼ��رգ�����ͷ��ڴ�
    sem_unlink(__PUMP_NAME_IN_GLOBALSEM(tmp_sem));
    sem_close(__PUMP_SEM_IN_GLOBALSEM(tmp_sem));
    free(tmp_sem);

    return PUMP_OK;
#   else
    return sem_destroy(pSem);
#   endif
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemWait(pump_sem_t* pSem)
{
#if (defined PUMP_OS_WINDOWS)
    if (!pSem)
    {
        return PUMP_ERROR;
    }

#if defined PUMP_OS_WINPHONE
    if (WAIT_OBJECT_0 == WaitForSingleObjectEx(*pSem, INFINITE, FALSE))
#else
    if (WAIT_OBJECT_0 == WaitForSingleObject(*pSem, INFINITE))
#endif
    {
        return PUMP_OK;
    }

    return PUMP_ERROR;
#elif (defined PUMP_OS_POSIX)
    if (!pSem)
    {
        return PUMP_ERROR;
    }

#if defined (PUMP_OS_APPLE)
    return sem_wait(__PUMP_SEM_IN_GLOBALSEM(*pSem));
#else
    int retVal = PUMP_OK;
    while ((retVal = sem_wait(pSem)) != PUMP_OK && EINTR == errno);
    return retVal;
#endif
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemTimedWait(pump_sem_t* pSem, pump_uint32_t nTimeOut)
{
#if (defined PUMP_OS_WINDOWS)
    if (!pSem)
    {
        return PUMP_ERROR;
    }

#if defined PUMP_OS_WINPHONE
    if (WAIT_OBJECT_0 == WaitForSingleObjectEx(*pSem, nTimeOut, FALSE))
#else
    if (WaitForSingleObject(*pSem, nTimeOut) == WAIT_OBJECT_0)
#endif
    {
        return PUMP_OK;
    }

    return PUMP_ERROR;
#elif (defined PUMP_OS_POSIX)
    if ( !pSem )
    {
        return PUMP_ERROR;
    }

    if(nTimeOut == 0)
    {
#if defined(PUMP_OS_APPLE)
        return sem_trywait(__PUMP_SEM_IN_GLOBALSEM(*pSem));    
#else
        int retVal = PUMP_OK;
        while( (retVal = sem_trywait(pSem)) != PUMP_OK && EINTR == errno);
        return retVal;
#endif
    }

#if defined (PUMP_OS_LINUX)
    timeval tv = {0};
    timespec ts = {0};
    pump_int64_t tNow;

    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + nTimeOut/1000;

    tNow = tv.tv_usec;
    tNow *= 1000;
    tNow += (nTimeOut%1000)*1000*1000;
    if (tNow > 999999999)
    {
        ts.tv_sec++;
    }
    ts.tv_nsec = tNow%(1000*1000*1000);

    int retVal = PUMP_OK;
    while( (retVal = sem_timedwait(pSem, &ts)) != PUMP_OK && EINTR == errno);
    return retVal;
#elif defined (PUMP_OS_APPLE)
    //Mac OS sem_tiemdwait, TODO 20200923 ��ʵ��
    // return SemTimedWait(&(__PUMP_SEM_IN_GLOBALSEM(*pSem)), nTimeOut);
#endif
    return PUMP_ERROR;
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_SemPost(pump_sem_t* pSem)
{
#if (defined PUMP_OS_WINDOWS)
    if (!pSem)
    {
        return PUMP_ERROR;
    }

    return (ReleaseSemaphore(*pSem, 1, NULL)) ? PUMP_OK : PUMP_ERROR;
#elif (defined PUMP_OS_POSIX)
    if (!pSem)
    {
        return PUMP_ERROR;
    }
#if defined (PUMP_OS_APPLE)
    return sem_post(__PUMP_SEM_IN_GLOBALSEM(*pSem));
#else
    return sem_post(pSem);
#endif
#endif // (defined PUMP_OS_WINDOWS)
}

////////////////////////////////////////////////////////////
// class CSemaphore
////////////////////////////////////////////////////////////
namespace Pump
{
namespace Core
{
namespace Thread
{

CSemaphore::CSemaphore(pump_uint32_t nInitCount, pump_uint32_t nMaxCount)
#ifdef PUMP_OS_POSIX
    : m_nMaxCount(nMaxCount)
#   ifdef PUMP_OS_APPLE
    , m_nCurCount(nInitCount)
#   endif // PUMP_OS_APPLE
#endif // PUMP_OS_POSIX
{
#if (defined PUMP_OS_WINDOWS)
#if defined PUMP_OS_WINPHONE
    m_sem = CreateSemaphoreExW(NULL, nInitCount, nMaxCount, NULL, 0, SYNCHRONIZE | SEMAPHORE_MODIFY_STATE);
#else
    m_sem = CreateSemaphore(NULL, nInitCount, nMaxCount, NULL);
#endif
#elif (defined PUMP_OS_POSIX)
    PUMP_CORE_SemCreate(&m_sem, nInitCount);
#endif // (defined PUMP_OS_WINDOWS)
}

CSemaphore::~CSemaphore()
{
    PUMP_CORE_SemDestroy(&m_sem);
}

pump_int32_t CSemaphore::Wait()
{
#ifdef PUMP_OS_APPLE

    if (PUMP_CORE_SemWait(&m_sem) == PUMP_OK)
    {
        m_csLock.Lock();
        --m_nCurCount;
        m_csLock.Unlock();
        return PUMP_OK;
    }
    else
    {
        return PUMP_ERROR;
    }

#else
    return PUMP_CORE_SemWait(&m_sem);
#endif
}

pump_int32_t CSemaphore::TimedWait(pump_uint32_t nTimeOut)
{
#ifdef PUMP_OS_APPLE

    if (PUMP_CORE_SemTimedWait(&m_sem, nTimeOut) == PUMP_OK)
    {
        m_csLock.Lock();
        --m_nCurCount;
        m_csLock.Unlock();
        return PUMP_OK;
    }
    else
    {
        return PUMP_ERROR;
    }
#else
    return PUMP_CORE_SemTimedWait(&m_sem, nTimeOut);
#endif
}

pump_int32_t CSemaphore::Post()
{
#if (defined PUMP_OS_POSIX)
#ifdef PUMP_OS_APPLE

    m_csLock.Lock();
    if (m_nCurCount >= m_nMaxCount)
    {
        m_csLock.Unlock();
        return PUMP_ERROR;
    }
    if (PUMP_CORE_SemPost(&m_sem) == PUMP_OK)
    {
        ++ m_nCurCount;
    }
    m_csLock.Unlock();
    return PUMP_OK;

#else

    register int iCurCnt = 0;

    m_csLock.Lock();
    sem_getvalue(&m_sem, &iCurCnt);
    if (iCurCnt >= (int)m_nMaxCount)
    {
        m_csLock.Unlock();
        return PUMP_ERROR;
    }

    int iRet = PUMP_CORE_SemPost(&m_sem);
    m_csLock.Unlock();

    return iRet;

#endif
#else
    return PUMP_CORE_SemPost(&m_sem);
#endif
}

}
}
}