#include "pump_http/__pump_http_global_ctrl.h"
#include "pump_core/pump_core_thread_pool.h"

namespace Pump
{
namespace Http
{

__CPumpHttpGlobalCtrl * __CPumpHttpGlobalCtrl::s_pGlobalCtrl = NULL;
::Pump::Core::Thread::CMutex __CPumpHttpGlobalCtrl::s_csGlobalCtrl;

__CPumpHttpGlobalCtrl::__CPumpHttpGlobalCtrl()
    : m_hWorkingThxPool(PUMP_INVALID_THREADPOOL)
{
}

__CPumpHttpGlobalCtrl::~__CPumpHttpGlobalCtrl()
{
}

pump_int32_t __CPumpHttpGlobalCtrl::Init()
{
    if (__CPumpHttpGlobalCtrl::s_pGlobalCtrl)
    {
        return PUMP_OK;
    }
    __CPumpHttpGlobalCtrl::s_csGlobalCtrl.Lock();
    if (__CPumpHttpGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpHttpGlobalCtrl::s_csGlobalCtrl.Unlock();
        return PUMP_OK;
    }
    __CPumpHttpGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) __CPumpHttpGlobalCtrl();
    if (!__CPumpHttpGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpHttpGlobalCtrl::s_csGlobalCtrl.Unlock();
        return PUMP_ERROR;
    }
    __CPumpHttpGlobalCtrl::s_csGlobalCtrl.Unlock();
    __CPumpHttpGlobalCtrl::s_pGlobalCtrl->CreateWorkingThxPool();
    return PUMP_OK;
}

pump_int32_t __CPumpHttpGlobalCtrl::Cleanup()
{
    if (!__CPumpHttpGlobalCtrl::s_pGlobalCtrl)
    {
        return PUMP_OK;
    }
    __CPumpHttpGlobalCtrl::s_csGlobalCtrl.Lock();
    delete __CPumpHttpGlobalCtrl::s_pGlobalCtrl;
    __CPumpHttpGlobalCtrl::s_pGlobalCtrl = NULL;
    __CPumpHttpGlobalCtrl::s_csGlobalCtrl.Unlock();
    return PUMP_OK;
}

__CPumpHttpGlobalCtrl * __CPumpHttpGlobalCtrl::GetGlobalCtrlLocked()
{
    s_csGlobalCtrl.Lock();
    if (__CPumpHttpGlobalCtrl::s_pGlobalCtrl)
    {
        return __CPumpHttpGlobalCtrl::s_pGlobalCtrl;
    }
    s_csGlobalCtrl.Unlock();
    if (__CPumpHttpGlobalCtrl::Init() == PUMP_ERROR)
    {
        s_csGlobalCtrl.Unlock();
        return NULL;
    }
    return __CPumpHttpGlobalCtrl::s_pGlobalCtrl;
}

void __CPumpHttpGlobalCtrl::GlobalCtrlUnlocked()
{
    s_csGlobalCtrl.Unlock();
}

pump_int32_t __CPumpHttpGlobalCtrl::CreateWorkingThxPool()
{
    if (PUMP_INVALID_THREADPOOL== m_hWorkingThxPool)
    {
        m_csWorkingThxPool.Lock();
        PUMP_CORE_InitThreadPool();
        m_hWorkingThxPool = PUMP_CORE_ThreadPool_Create(10, 100);
        if (m_hWorkingThxPool == PUMP_INVALID_THREADPOOL)
        {
            m_csWorkingThxPool.Unlock();
            return PUMP_ERROR;
        }
        m_csWorkingThxPool.Unlock();
    }
    return PUMP_OK;
}

pump_int32_t __CPumpHttpGlobalCtrl::DestroyWorkingThxPool()
{
    if (PUMP_INVALID_THREADPOOL != m_hWorkingThxPool)
    {
        m_csWorkingThxPool.Lock();
        PUMP_CORE_ThreadPool_Destroy(m_hWorkingThxPool);
        m_hWorkingThxPool = PUMP_INVALID_THREADPOOL;
        PUMP_CORE_FiniThreadPool();
        m_csWorkingThxPool.Unlock();
    }
    return PUMP_OK;
}

pump_handle_t __CPumpHttpGlobalCtrl::GetWorkingThxPoolLocked()
{
    m_csWorkingThxPool.Lock();
    if (PUMP_INVALID_THREADPOOL == m_hWorkingThxPool)
    {
        m_csWorkingThxPool.Unlock();
        return NULL;
    }
    return m_hWorkingThxPool;
}

void __CPumpHttpGlobalCtrl::WorkingThxPoolUnlock()
{
    m_csWorkingThxPool.Unlock();
}

pump_int32_t __CPumpHttpGlobalCtrl::__Init()
{
    return 0;
}
pump_int32_t __CPumpHttpGlobalCtrl::__Cleanup()
{
    return 0;
}

}
}