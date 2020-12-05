/**
 * @file pump_core_thread.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of thread control interface.
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

#include <string>
#ifdef PUMP_CORE_HAVE_BOOST
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#endif // PUMP_CORE_HAVE_BOOST
#include "pump_core/pump_core_object.h"
#include "pump_core/thread/pump_core_thread.h"

namespace Pump
{
namespace Core
{
namespace Thread
{

class __CThreadPrimitive
    : public ::Pump::Core::CPrimitiveBase
{
public:
    friend class CThread;
public:
    __CThreadPrimitive();
    virtual ~__CThreadPrimitive();
    virtual pump_void_t Bind(pump_void_t* cb, pump_void_t * pData) = 0;
    virtual pump_int32_t Start() = 0;
    virtual pump_int32_t Stop() = 0;
    virtual pump_int32_t Suspend() = 0;
    virtual pump_int32_t Resume() = 0;
    virtual pump_int32_t  SetPriority(pump_int32_t iPriority) = 0;
    virtual pump_int32_t  SetSchedPolicy(pump_int32_t iSchedPolicy) = 0;
    virtual pump_thread_id GetTID() = 0;
    virtual pump_void_t * GetData() = 0;
public:
    pump_thread_id m_TID;
};

__CThreadPrimitive::__CThreadPrimitive()
    : ::Pump::Core::CPrimitiveBase()
    , m_TID((unsigned long int)PUMP_INVALID_THREAD)
{

}

__CThreadPrimitive::~__CThreadPrimitive()
{
}

#ifdef PUMP_CORE_HAVE_BOOST
class __CBoostThreadPrimitive
    : public __CThreadPrimitive
{
public:
    typedef pump_void_t * (*ThreadCallbackType)(pump_void_t * pData);
public:
    __CBoostThreadPrimitive()
        : __CThreadPrimitive()
        , m_pCb(PUMP_NULL)
        , m_pData(PUMP_NULL)
    {

    }
    virtual ~__CBoostThreadPrimitive()
    {
        this->Stop();
        m_pCb = PUMP_NULL;
        m_pData = PUMP_NULL;
    }
    virtual void Bind(void* cb, pump_void_t * pData)
    {
        m_pCb = (ThreadCallbackType)cb;
        m_pData = pData;
    }
    virtual pump_int32_t Start()
    {
        if (!m_pCb)
        {
            return PUMP_ERROR;
        }
        m_thx = boost::thread(m_pCb, m_pData);
        return PUMP_OK;
    }
    virtual pump_int32_t Stop()
    {
        m_thx.join();
        return PUMP_OK;
    }
    virtual pump_int32_t Suspend() { return PUMP_ERROR; }
    virtual pump_int32_t Resume() { return PUMP_ERROR; }
    virtual pump_int32_t  SetPriority(pump_int32_t iPriority) { return PUMP_ERROR; }
    virtual pump_int32_t  SetSchedPolicy(pump_int32_t iSchedPolicy) { return PUMP_ERROR; }
    virtual pump_thread_id GetTID()
    {
        if (m_TID!=PUMP_INVALID_THREAD)
        {
            return m_TID;
        }
        std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
        m_TID = std::stoul(threadId, PUMP_NULL, 16);
        return m_TID;
    }
    virtual pump_void_t * GetData()
    {
        return m_pData;
    }
private:
    boost::thread m_thx;
    ThreadCallbackType m_pCb;
    pump_void_t *m_pData;
};
#else 
class __CBuildinThreadPrimitive
    : public __CThreadPrimitive
{
public:
    typedef pump_void_t * (*ThreadCallbackType)(pump_void_t * pData);
public:
    __CBuildinThreadPrimitive()
        : __CThreadPrimitive()
        , m_pCb(PUMP_NULL)
        , m_pData(PUMP_NULL)
    {

    }
    virtual ~__CBuildinThreadPrimitive()
    {
        this->Stop();
        m_pCb = PUMP_NULL;
        m_pData = PUMP_NULL;
    }
    virtual void Bind(void* cb, pump_void_t * pData)
    {
        m_pCb = (ThreadCallbackType)cb;
        m_pData = pData;
    }
    virtual pump_int32_t Start()
    {
        if (!m_pCb)
        {
            return PUMP_ERROR;
        }
        m_thx = PUMP_CORE_Thread_Create(m_pCb, m_pData, 0);
        return PUMP_OK;
    }
    virtual pump_int32_t Stop()
    {
        return PUMP_CORE_Thread_Wait(m_thx);;
    }
    virtual pump_int32_t Suspend() { return PUMP_ERROR; }
    virtual pump_int32_t Resume() { return PUMP_ERROR; }
    virtual pump_int32_t  SetPriority(pump_int32_t iPriority) { return PUMP_ERROR; }
    virtual pump_int32_t  SetSchedPolicy(pump_int32_t iSchedPolicy) { return PUMP_ERROR; }
    virtual pump_thread_id GetTID()
    {
        if (m_TID != PUMP_INVALID_THREAD)
        {
            return m_TID;
        }
        m_TID = PUMP_CORE_Thread_GetSelfId();
        return m_TID;
    }
    virtual pump_void_t * GetData()
    {
        return m_pData;
    }
private:
    pump_handle_t m_thx;
    ThreadCallbackType m_pCb;
    pump_void_t *m_pData;
};
#endif // PUMP_CORE_HAVE_BOOST

CThread::CThread()
    : m_pData(PUMP_NULL)
#ifdef PUMP_CORE_HAVE_BOOST
    , m_pPrimitive(new(std::nothrow)__CBoostThreadPrimitive())
    , m_emType(CThread::PUMP_THREAD_BOOST)
#else
    ,m_pPrimitive(PUMP_NULL)
    , m_emType(CThread::PUMP_THREAD_DEFAULT)
#endif // PUMP_CORE_HAVE_BOOST
{
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    if (pPrimitive)
    {
        pPrimitive->Bind(CThread::__ThreadCallbackCore, this);
    }
}

CThread::CThread(pump_void_t * pData)
    : m_pData(pData)
#ifdef PUMP_CORE_HAVE_BOOST
    , m_pPrimitive(new(std::nothrow)__CBoostThreadPrimitive())
    , m_emType(CThread::PUMP_THREAD_BOOST)
#else
    , m_pPrimitive(PUMP_NULL)
    , m_emType(CThread::PUMP_THREAD_DEFAULT)
#endif // PUMP_CORE_HAVE_BOOST
{
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    if (pPrimitive)
    {
        pPrimitive->Bind(CThread::__ThreadCallbackCore, this);
    }
}

CThread::CThread(PUMP_THREAD_TYPE emType, pump_void_t * pData)
    : m_pData(pData)
    , m_pPrimitive(PUMP_NULL)
    , m_emType(emType)
{
    switch (m_emType)
    {
#ifdef PUMP_CORE_HAVE_BOOST
    case CThread::PUMP_THREAD_BOOST:
    {
        __CBoostThreadPrimitive * pPrimitive = new (std::nothrow)__CBoostThreadPrimitive();
        if (pPrimitive)
        {
            pPrimitive->Bind(CThread::__ThreadCallbackCore, pData);
        }
    } break;
#endif // PUMP_CORE_HAVE_BOOST
    default:
        break;
    }
}

/*FIXME 20200617 Couldn't ensure thread exit, need pthread_wait() like function.*/
CThread::~CThread()
{
    delete m_pPrimitive;
}

pump_int32_t CThread::Start()
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->Start();
}
pump_int32_t CThread::Stop()
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->Stop();
}

pump_int32_t CThread::Suspend()
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->Suspend();
}

pump_int32_t CThread::Resume()
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->Resume();
}

pump_int32_t CThread::SetPriority(pump_int32_t iPriority)
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->SetPriority(iPriority);
}

pump_int32_t CThread::SetSchedPolicy(pump_int32_t iSchedPolicy)
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->SetSchedPolicy(iSchedPolicy);
}

pump_thread_id CThread::GetTID()
{
    if (!m_pPrimitive)
    {
        return PUMP_ERROR;
    }
    __CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(m_pPrimitive);
    return pPrimitive->GetTID();
}

void CThread::SetData(void * pData)
{
    m_pData = pData;
}

pump_void_t * CThread::__ThreadCallbackCore(pump_void_t * pData)
{
    CThread * pthis = static_cast<CThread *>(pData);
    if (!pthis)
    {
        return (pump_void_t *)PUMP_ERROR;
    }
    //__CThreadPrimitive * pPrimitive = static_cast<__CThreadPrimitive*>(pthis->m_pPrimitive);
    return pthis->ThreadCallback(pthis->m_pData);
}

}
}
}

#ifdef PUMP_OS_WINPHONE
#include <thread>
#elif !(defined PUMP_OS_WINCE)
#include <process.h>
#endif

#ifndef TLS_OUT_OF_INDEXES
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
#endif

PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_Thread_Create(pump_pvoid_t(PUMP_CALLBACK *StartAddress)(pump_pvoid_t), pump_void_t* Params, pump_uint32_t StackSize, \
    pump_bool_t IsSuspend, pump_int32_t Priority, pump_int32_t SchedPolicy)
{
#if (defined PUMP_OS_WINDOWS)
    /*lint -save  -sem(_beginthreadex, custodial(4)) */
    unsigned int initflag = 0;
    if (IsSuspend)
    {
        initflag = CREATE_SUSPENDED;
    }

    if (StackSize != 0)
    {
        initflag = initflag | STACK_SIZE_PARAM_IS_A_RESERVATION;
    }

#   if defined PUMP_OS_WINCE
    return (pump_handle_t)CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)StartAddress, Params, initflag, NULL);
#   elif defined PUMP_OS_WINPHONE
    return new(std::nothrow) std::thread(StartAddress, Params);
#   else
    return (pump_handle_t)_beginthreadex(NULL, StackSize, (unsigned(__stdcall*)(void*))StartAddress, Params, initflag, NULL);
#   endif // defined PUMP_OS_WINCE
    /*lint -restore */
#elif (defined PUMP_OS_POSIX)
    pthread_attr_t threadattr;
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return (pump_handle_t)PUMP_INVALID_THREAD;
    }

#   if (!defined (PUMP_OS_APPLE)) //APPLE不支持
    //struct sched_param schedparam;
    //schedparam.sched_priority = SchedPolicy;
    ret = pthread_attr_setschedpolicy(&threadattr, SchedPolicy);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (pump_handle_t)PUMP_INVALID_THREAD;
    }
#   endif

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            //printf("pthread_attr_setstacksize error\n");
            pthread_attr_destroy(&threadattr);
            return (pump_handle_t)(PUMP_INVALID_THREAD);

        }
    }

#   if defined (PUMP_OS_POSIX)
    struct sched_param param;
#       if defined PUMP_OS_ANDROID
    param.sched_priority = Priority;
#       else
    param.__sched_priority = Priority;
#       endif
    ret = pthread_attr_setschedparam(&threadattr, &param);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (pump_handle_t)PUMP_INVALID_THREAD;
    }
#   endif

    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        return (pump_handle_t)PUMP_INVALID_THREAD;
    }
    else
    {
        return (pump_handle_t)threadid;
    }
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_ThreadDetached_Create(pump_pvoid_t(PUMP_CALLBACK *StartAddress)(pump_pvoid_t), pump_void_t* Params, pump_uint32_t StackSize)
{
#if (defined PUMP_OS_WINDOWS)
#   if defined PUMP_OS_WINPHONE
    std::thread thr(StartAddress, Params);
    thr.detach();

    return PUMP_TRUE;
#   else // defined PUMP_OS_WINPHONE
    HANDLE hThreadId;
#           if defined PUMP_OS_WINCE
    hThreadId = (pump_handle_t)CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)StartAddress, Params, 0, NULL);
#           else // defined PUMP_OS_WINCE
    unsigned int initflag = 0;
    if (StackSize != 0)
    {
        initflag = initflag | STACK_SIZE_PARAM_IS_A_RESERVATION;
    }
    hThreadId = (pump_handle_t)_beginthreadex(NULL, StackSize, (unsigned(__stdcall*)(void*))StartAddress, Params, initflag, NULL);
#           endif // defined PUMP_OS_WINCE
    if (hThreadId != NULL)
    {
        CloseHandle(hThreadId);
        //hThreadId = NULL;
        return PUMP_TRUE;
    }

    return PUMP_FALSE;
#   endif // defined PUMP_OS_WINPHONE
#elif (defined PUMP_OS_POSIX)
    pthread_attr_t threadattr;
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        //return (pump_handle_t)PUMP_INVALID_THREAD;
        return PUMP_FALSE;
    }
    ret = pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        //return (pump_handle_t)PUMP_INVALID_THREAD;
        return PUMP_FALSE;
    }

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            pthread_attr_destroy(&threadattr);
            return PUMP_FALSE;

        }
    }

    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        //return (pump_handle_t)PUMP_INVALID_THREAD;
        return PUMP_FALSE;
    }
    else
    {
        //return (pump_handle_t)threadid;
        return PUMP_TRUE;
    }
#endif // defined PUMP_OS_POSIX
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Exit()
{
#if (defined PUMP_OS_WINDOWS)
#   if !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    _endthread();
#   endif // !defined PUMP_OS_WINCE && !defined PUMP_OS_WINPHONE
    return 0;
#elif (defined PUMP_OS_POSIX)
    pthread_exit(0);
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Wait(pump_handle_t ThreadHandle)
{
#if (defined PUMP_OS_WINDOWS)
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }
#   if (defined PUMP_OS_WINPHONE)
    std::thread* thr = (std::thread*)ThreadHandle;
    thr->join();
    delete thr;
    thr = NULL;

    return PUMP_OK;
#   else
    if (WAIT_OBJECT_0 == WaitForSingleObject(ThreadHandle, INFINITE))
    {
        CloseHandle(ThreadHandle);
        return PUMP_OK;
    }
    return PUMP_ERROR;
#   endif
#elif (defined PUMP_OS_POSIX)
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }

    int ret = pthread_join((pthread_t)ThreadHandle, 0);
    if (ret)
    {
        return PUMP_ERROR;
    }
    else
    {
        //close((int)ThreadHandle);
        return PUMP_OK;
    }
#endif // defined PUMP_OS_WINDOWS
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Suspend(pump_handle_t ThreadHandle)
{
#if (defined PUMP_OS_WINDOWS)
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }
#   if defined PUMP_OS_WINPHONE
    return PUMP_ERROR;
#   else
    DWORD ret = SuspendThread(ThreadHandle);
    if (ret == 0xFFFFFFFF)
    {
        return PUMP_ERROR;
    }
    else
    {
        return PUMP_OK;
    }
#   endif // defined PUMP_OS_WINPHONE
#elif (defined PUMP_OS_POSIX)
    return PUMP_ERROR; // 不支持
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_Resume(pump_handle_t ThreadHandle)
{
#if (defined PUMP_OS_WINDOWS)
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }
#   if defined PUMP_OS_WINPHONE
    return PUMP_ERROR;
#   else
    DWORD ret = ResumeThread(ThreadHandle);
    if (ret == 0xFFFFFFFF)
    {
        return PUMP_ERROR;
    }
    else
    {
        return PUMP_OK;
    }
#   endif
#elif (defined PUMP_OS_POSIX)
    return PUMP_ERROR; // 不支持
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_SetPriority(pump_handle_t ThreadHandle, pump_int32_t Priority)
{
#if (defined PUMP_OS_WINDOWS)
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }
#   if defined PUMP_OS_WINPHONE
    return PUMP_ERROR;
#   else
    BOOL bret = SetThreadPriority(ThreadHandle, Priority);
    if (bret)
    {
        return PUMP_OK;
    }
    else
    {
        return PUMP_ERROR;
    }
#   endif
#elif (defined PUMP_OS_POSIX)
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }

    int policy;
    struct sched_param param;
    int ret = pthread_getschedparam((pthread_t)ThreadHandle, &policy, &param);
    if (ret)
    {
        return PUMP_ERROR;
    }

#   if defined (__linux__)
#       if defined OS_ANDROID
    param.sched_priority = Priority;
#       else
    param.__sched_priority = Priority;
#       endif
    ret = pthread_setschedparam((pthread_t)ThreadHandle, policy, &param);
    if (ret)
    {
        return PUMP_ERROR;
    }
    else
    {
        return PUMP_OK;
    }
#   endif
    return PUMP_OK;
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_Thread_SetSchedPolicy(pump_handle_t ThreadHandle, pump_int32_t SchedPolicy)
{
    if (ThreadHandle == (pump_handle_t)PUMP_INVALID_THREAD)
    {
        return PUMP_ERROR;
    }
#if !defined PUMP_OS_WINCE && !defined PUMP_OS_WINPHONE
    //WinCE没有SetPriorityClass这个函数
    BOOL bret = SetPriorityClass(GetCurrentProcess(), SchedPolicy);
    if (bret)
    {
        return PUMP_OK;
    }
    else
    {
        return PUMP_ERROR;
    }
#else
    return PUMP_ERROR;
#endif
}

PUMP_CORE_API pump_thread_id PUMP_CALLBACK PUMP_CORE_Thread_GetSelfId()
{
#if (defined PUMP_OS_WINDOWS)
    return ::GetCurrentThreadId();
#elif (defined PUMP_OS_POSIX)
    return pthread_self();
#endif // (defined PUMP_OS_WINDOWS)
}

//////////////////////////////////////////////////////////////////////////
// Thread Local Storage API.
//////////////////////////////////////////////////////////////////////////
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_ThreadTls_Create()
{
#if (defined PUMP_OS_WINDOWS)
#   if defined PUMP_OS_WINPHONE
    return NULL;
#   else
    pump_uint32_t tls_index = TlsAlloc();
    if (tls_index == TLS_OUT_OF_INDEXES)
    {
        return PUMP_INVALID_TLS;
    }

    return (pump_handle_t)(tls_index);
#   endif
#elif (defined PUMP_OS_POSIX)
    pthread_key_t tls_key;
    if (pthread_key_create(&tls_key, NULL) != 0)
    {
        return PUMP_INVALID_TLS;
    }
    return (pump_handle_t)(tls_key);
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadTls_SetValue(pump_handle_t hTls, pump_pvoid_t pVal)
{
#if (defined PUMP_OS_WINDOWS)
    if (hTls == PUMP_INVALID_TLS)
    {
        return PUMP_ERROR;
    }

    pump_uint32_t tls_index = (pump_uint32_t)(LPARAM)hTls;
#   if defined PUMP_OS_WINPHONE
    return PUMP_ERROR;
#   else
    return TlsSetValue(tls_index, pVal) ? PUMP_OK : PUMP_ERROR;
#   endif
#elif (defined PUMP_OS_POSIX)
    if (hTls == PUMP_INVALID_TLS)
    {
        return PUMP_ERROR;
    }

    pthread_key_t tls_key = (pthread_key_t)(long)hTls;

    return (pthread_setspecific(tls_key, pVal) == 0) ? PUMP_OK : PUMP_ERROR;
#endif 
}

PUMP_CORE_API pump_pvoid_t PUMP_CALLBACK PUMP_CORE_ThreadTls_GetValue(pump_handle_t hTls)
{
#if (defined PUMP_OS_WINDOWS)
    if (hTls == PUMP_INVALID_TLS)
    {
        return NULL;
    }

    pump_uint32_t tls_index = (pump_uint32_t)(LPARAM)hTls;
#   if defined PUMP_OS_WINPHONE
    return NULL;
#   else
    return TlsGetValue(tls_index);
#   endif
#elif (defined PUMP_OS_POSIX)
    if (hTls == PUMP_INVALID_TLS)
    {
        return NULL;
    }

    pthread_key_t tls_key = (pthread_key_t)(long)hTls;

    return pthread_getspecific(tls_key);
#endif 
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_ThreadTls_Destroy(pump_handle_t hTls)
{
#if (defined PUMP_OS_WINDOWS)
    if (hTls == PUMP_INVALID_TLS)
    {
        return PUMP_ERROR;
    }

    pump_uint32_t tls_index = (pump_uint32_t)(LPARAM)hTls;
#   if defined PUMP_OS_WINPHONE
    return PUMP_ERROR;
#   else
    return TlsFree(tls_index) ? PUMP_OK : PUMP_ERROR;
#   endif
#elif (defined PUMP_OS_POSIX)
    if (hTls == PUMP_INVALID_TLS)
    {
        return PUMP_ERROR;
    }
    pthread_key_t tls_key = (pthread_key_t)(long)hTls;
    return (pthread_key_delete(tls_key) == 0) ? PUMP_OK : PUMP_ERROR;
#endif
}