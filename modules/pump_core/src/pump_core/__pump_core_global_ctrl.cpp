/**
 * @file __pump_core_global_ctrl.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of pump_core global resource control interface.
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

#include "pump_core/__pump_core_global_ctrl.h"

namespace Pump
{
namespace Core
{

/***********************************************
* __CPumpCoreGlobalCtrlGuider
***********************************************/
//__CPumpCoreGlobalCtrlGuider::__CPumpCoreGlobalCtrlGuider()
//    : m_pPumpCoreGlobalCtrl(NULL)
//{
//    __CPumpCoreGlobalCtrl::ReadLock();
//}

__CPumpCoreGlobalCtrlGuider::__CPumpCoreGlobalCtrlGuider(__CPumpCoreGlobalCtrl * pPumpCoreGlobalCtrl)
    : m_pPumpCoreGlobalCtrl(pPumpCoreGlobalCtrl)
{
    __CPumpCoreGlobalCtrl::ReadLock();
}

__CPumpCoreGlobalCtrlGuider::~__CPumpCoreGlobalCtrlGuider()
{
    __CPumpCoreGlobalCtrl::ReadUnlock();
}

__CPumpCoreGlobalCtrlGuider::__CPumpCoreGlobalCtrlGuider(__CPumpCoreGlobalCtrlGuider & other)
    : m_pPumpCoreGlobalCtrl(other.m_pPumpCoreGlobalCtrl)
{
    __CPumpCoreGlobalCtrl::ReadUnlock();
}

__CPumpCoreGlobalCtrl * __CPumpCoreGlobalCtrlGuider::GetPumpCoreGlobalCtrl()
{
    return m_pPumpCoreGlobalCtrl;
}

/***********************************************
 * __CPumpCoreGlobalCtrl
 ***********************************************/
__CPumpCoreGlobalCtrl::__CPumpCoreGlobalCtrl()
    : CGlobalCtrlBase()
    , m_pRecorderMgr(PUMP_NULL)
    , m_pCmdSessionMgr(PUMP_NULL)
{
}

__CPumpCoreGlobalCtrl::~__CPumpCoreGlobalCtrl()
{
    this->__Cleanup();
}

pump_int32_t __CPumpCoreGlobalCtrl::__Init()
{
    /** Initialize log recorder manger which must init before m_pPumpCoreLogRecorder*/
    if (!m_pRecorderMgr)
    {
        m_csRecorderMgr.Lock();
        if (!m_pRecorderMgr)
        {
            m_pRecorderMgr
                = new (std::nothrow) ::Pump::Core::CLogRecorderMgr();
            m_csRecorderMgr.Unlock();
            if (!m_pRecorderMgr)
            {
                __Cleanup();
                return PUMP_ERROR;
            }
        }
    }

    /** Initialize CMD session manger. */
    if (!m_pCmdSessionMgr)
    {
        m_csCmdSessionMgr.Lock();
        if (!m_pCmdSessionMgr)
        {
            m_pCmdSessionMgr
                = new  (std::nothrow) ::Pump::Core::Cmder::CCmdSessionMgr();
            m_csCmdSessionMgr.Unlock();
            if (!m_pCmdSessionMgr)
            {
                __Cleanup();
                return PUMP_ERROR;
            }
        }
    }

    /** Set init flag. */
    this->SetInitFlag(PUMP_TRUE);
    return PUMP_OK;
}

pump_int32_t __CPumpCoreGlobalCtrl::__Cleanup()
{
    if(m_pCmdSessionMgr)
    {
        m_csCmdSessionMgr.Lock();
        delete m_pCmdSessionMgr;
        m_pCmdSessionMgr = NULL;
        m_csCmdSessionMgr.Unlock();
    }
    if (m_pRecorderMgr)
    {
        m_csRecorderMgr.Lock();
        delete m_pRecorderMgr;
        m_pRecorderMgr = NULL;
        m_csRecorderMgr.Unlock();
    }
    this->SetInitFlag(PUMP_FALSE);
    return PUMP_OK;
}

/** Object Create. */
pump_int32_t __CPumpCoreGlobalCtrl::Create()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return PUMP_OK;
    }
    // double protection lock, 
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        __CPumpCoreGlobalCtrl::WriteLock();
        if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::WriteUnlock();
            return PUMP_OK;
        }
        else
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) ::Pump::Core::__CPumpCoreGlobalCtrl();
            __CPumpCoreGlobalCtrl::WriteUnlock();
            if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
            {
                return PUMP_ERROR;
            }
        }
    }
    return PUMP_OK;
}

/** Object Destroy. */
pump_int32_t __CPumpCoreGlobalCtrl::Destroy()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return PUMP_OK;
    }
    // double protection lock, 
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        __CPumpCoreGlobalCtrl::WriteLock();
        if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::WriteUnlock();
            return PUMP_OK;
        }
        else
        {
            delete __CPumpCoreGlobalCtrl::s_pGlobalCtrl;
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = NULL;
            __CPumpCoreGlobalCtrl::WriteUnlock();
            if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
            {
                return PUMP_ERROR;
            }
        }
    }
    return PUMP_OK;
}

pump_int32_t __CPumpCoreGlobalCtrl::Init()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (__CPumpCoreGlobalCtrl::IsInit())
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return PUMP_OK;
    }

    int ret = __PUMP_CORE_GLOBAL_CTRL2()->__Init();
    __CPumpCoreGlobalCtrl::ReadUnlock();
    return ret;
}

pump_int32_t __CPumpCoreGlobalCtrl::Cleanup()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (!__PUMP_CORE_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return PUMP_ERROR;
    }
    __PUMP_CORE_GLOBAL_CTRL2()->__Cleanup();
    __CPumpCoreGlobalCtrl::ReadUnlock();
    return PUMP_OK;
}

__CPumpCoreGlobalCtrlGuider __CPumpCoreGlobalCtrl::GetGlobalCtrl()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl && __CPumpCoreGlobalCtrl::s_pGlobalCtrl->IsInit())
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return __CPumpCoreGlobalCtrlGuider(__PUMP_CORE_GLOBAL_CTRL2());
    }
    // double protection lock, 
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::WriteLock();
        if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::WriteUnlock();
            __CPumpCoreGlobalCtrl::ReadUnlock();
            return __CPumpCoreGlobalCtrlGuider(__PUMP_CORE_GLOBAL_CTRL2());
        }
        else
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) ::Pump::Core::__CPumpCoreGlobalCtrl();
            __CPumpCoreGlobalCtrl::WriteUnlock();
            if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
            {
                __CPumpCoreGlobalCtrl::ReadUnlock();
                return __CPumpCoreGlobalCtrlGuider(NULL);
            }
            __PUMP_CORE_GLOBAL_CTRL2()->Init();
        }
    }
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->IsInit())
    {
        __PUMP_CORE_GLOBAL_CTRL2()->Init();
    }
    __CPumpCoreGlobalCtrl::ReadUnlock();
    return __CPumpCoreGlobalCtrlGuider(__PUMP_CORE_GLOBAL_CTRL2());
}

CLogRecorderMgr * __CPumpCoreGlobalCtrl::GetLoggerMgr()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (__PUMP_CORE_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return __PUMP_CORE_GLOBAL_CTRL2()->m_pRecorderMgr;
    }
    __CPumpCoreGlobalCtrl::ReadUnlock();
    return NULL;
}

::Pump::Core::Cmder::CCmdSessionMgr * __CPumpCoreGlobalCtrl::GetCmdSessionMgr()
{
    __CPumpCoreGlobalCtrl::ReadLock();
    if (__PUMP_CORE_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpCoreGlobalCtrl::ReadUnlock();
        return __PUMP_CORE_GLOBAL_CTRL2()->m_pCmdSessionMgr;
    }
    __CPumpCoreGlobalCtrl::ReadUnlock();
    return NULL;
}

}
}