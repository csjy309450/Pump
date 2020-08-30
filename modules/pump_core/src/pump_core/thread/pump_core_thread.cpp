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
    : public ::Pump::Core::__CPrimitiveBase
{
public:
    friend class CThread;
public:
    __CThreadPrimitive();
    virtual ~__CThreadPrimitive();
    virtual pump_void_t Bind(pump_void_t* cb, pump_void_t * pData) = 0;
    virtual pump_int32_t Start() = 0;
    virtual pump_int32_t Stop() = 0;
    virtual pump_thread_id GetTID() = 0;
    virtual pump_void_t * GetData() = 0;
public:
    pump_thread_id m_TID;
};

__CThreadPrimitive::__CThreadPrimitive()
    : ::Pump::Core::__CPrimitiveBase()
    , m_TID(PUMP_INVALID_THREAD)
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
        pPrimitive->Bind((void*)CThread::__ThreadCallbackCore, this);
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
        pPrimitive->Bind((void*)CThread::__ThreadCallbackCore, this);
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
            pPrimitive->Bind((void*)CThread::__ThreadCallbackCore, pData);
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

pump_thread_id CThread::GetTID()
{
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