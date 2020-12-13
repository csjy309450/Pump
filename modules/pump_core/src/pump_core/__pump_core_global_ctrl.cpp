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

__CPumpCoreGlobalCtrl * __CPumpCoreGlobalCtrl::s_pGlobalCtrl = PUMP_NULL;
::Pump::Core::Thread::CRWLocker __CPumpCoreGlobalCtrl::s_csGlobalCtrl;

#define __PUMP_CORE_GLOBAL_CTRL2() (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)

PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadLock()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
}

PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadUnlock()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
}

/***********************************************
* __CPumpCoreGlobalCtrlKeeper
***********************************************/
__CPumpCoreGlobalCtrlKeeper::__CPumpCoreGlobalCtrlKeeper(__CPumpCoreGlobalCtrl * pGlobalCtrl)
    : __CPumpCoreGlobalResouceKeeper<__CPumpCoreGlobalCtrl>(pGlobalCtrl)
{}

__CPumpCoreGlobalCtrlKeeper::~__CPumpCoreGlobalCtrlKeeper()
{}

__CPumpCoreGlobalCtrlKeeper::__CPumpCoreGlobalCtrlKeeper(__CPumpCoreGlobalCtrlKeeper & other)
    : __CPumpCoreGlobalResouceKeeper<__CPumpCoreGlobalCtrl>(other)
{}

/***********************************************
* __CPumpCoreLogRecorderKeeper
***********************************************/
__CPumpCoreLogRecorderKeeper::__CPumpCoreLogRecorderKeeper(CLogRecorderBase* pLogRecorder)
    : __CPumpCoreGlobalResouceKeeper<CLogRecorderBase>(pLogRecorder)
{
    if (!__PUMP_CORE_GLOBAL_CTRL2() || !pLogRecorder)
    {
        return;
    }
    __PUMP_CORE_GLOBAL_CTRL2()->LogRecorderReadLock();
}

__CPumpCoreLogRecorderKeeper::__CPumpCoreLogRecorderKeeper(__CPumpCoreLogRecorderKeeper & other)
    : __CPumpCoreGlobalResouceKeeper<CLogRecorderBase>(other)
{
    if (!__PUMP_CORE_GLOBAL_CTRL2() || !other.GetPtr())
    {
        return;
    }
    __PUMP_CORE_GLOBAL_CTRL2()->LogRecorderReadLock();
}

__CPumpCoreLogRecorderKeeper::~__CPumpCoreLogRecorderKeeper()
{
    if (!__PUMP_CORE_GLOBAL_CTRL2() || !this->GetPtr())
    {
        return;
    }
    __PUMP_CORE_GLOBAL_CTRL2()->LogRecorderReadUnlock();
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

/** Set private logger. */
pump_int32_t __CPumpCoreGlobalCtrl::SetLogger(pump_handle_t hLogger)
{
    if (!hLogger)
    {
        return PUMP_ERROR;
    }
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (!s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
        return PUMP_OK;
    }
    s_pGlobalCtrl->SetLoggerPtr(hLogger);

    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return PUMP_OK;
}

/** Get private logger. */
__CPumpCoreLogRecorderKeeper __CPumpCoreGlobalCtrl::GetLogger()
{
    return __CPumpCoreLogRecorderKeeper((CLogRecorderBase*)(!s_pGlobalCtrl ? NULL : s_pGlobalCtrl->GetLoggerPtr()));
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
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_OK;
    }
    // double protection lock, 
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        __CPumpCoreGlobalCtrl::GlobalCtrlWriteLock();
        if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock();
            return PUMP_OK;
        }
        else
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) ::Pump::Core::__CPumpCoreGlobalCtrl();
            __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock();
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
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_OK;
    }
    // double protection lock, 
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        __CPumpCoreGlobalCtrl::GlobalCtrlWriteLock();
        if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock();
            return PUMP_OK;
        }
        else
        {
            delete __CPumpCoreGlobalCtrl::s_pGlobalCtrl;
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = NULL;
            __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock();
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
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (__CPumpCoreGlobalCtrl::IsInit())
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_OK;
    }

    int ret = __PUMP_CORE_GLOBAL_CTRL2()->__Init();
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return ret;
}

pump_bool_t __CPumpCoreGlobalCtrl::IsInit()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (!s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_FALSE;
    }
    pump_bool_t bInit = s_pGlobalCtrl->GetInitFlag();
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return bInit;
}

pump_int32_t __CPumpCoreGlobalCtrl::Cleanup()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (!__PUMP_CORE_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_ERROR;
    }
    __PUMP_CORE_GLOBAL_CTRL2()->__Cleanup();
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return PUMP_OK;
}

__CPumpCoreGlobalCtrlKeeper __CPumpCoreGlobalCtrl::GetGlobalCtrl()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl && __CPumpCoreGlobalCtrl::s_pGlobalCtrl->IsInit())
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return __CPumpCoreGlobalCtrlKeeper(__PUMP_CORE_GLOBAL_CTRL2());
    }
    // double protection lock, 
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlWriteLock();
        if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock();
            __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
            return __CPumpCoreGlobalCtrlKeeper(__PUMP_CORE_GLOBAL_CTRL2());
        }
        else
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) ::Pump::Core::__CPumpCoreGlobalCtrl();
            __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock();
            if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
            {
                __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
                return __CPumpCoreGlobalCtrlKeeper(NULL);
            }
            __PUMP_CORE_GLOBAL_CTRL2()->Init();
        }
    }
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->IsInit())
    {
        __PUMP_CORE_GLOBAL_CTRL2()->Init();
    }
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return __CPumpCoreGlobalCtrlKeeper(__PUMP_CORE_GLOBAL_CTRL2());
}

CLogRecorderMgr * __CPumpCoreGlobalCtrl::GetLoggerMgr()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (__PUMP_CORE_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return __PUMP_CORE_GLOBAL_CTRL2()->m_pRecorderMgr;
    }
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return NULL;
}

::Pump::Core::Cmder::CCmdSessionMgr * __CPumpCoreGlobalCtrl::GetCmdSessionMgr()
{
    __CPumpCoreGlobalCtrl::GlobalCtrlReadLock();
    if (__PUMP_CORE_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
        return __PUMP_CORE_GLOBAL_CTRL2()->m_pCmdSessionMgr;
    }
    __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock();
    return NULL;
}

void __CPumpCoreGlobalCtrl::GlobalCtrlReadLock()
{
    s_csGlobalCtrl.readLock();
}

void __CPumpCoreGlobalCtrl::GlobalCtrlReadUnlock()
{
    s_csGlobalCtrl.readUnlock();
}

void __CPumpCoreGlobalCtrl::GlobalCtrlWriteLock()
{
    s_csGlobalCtrl.writeLock();
}

void __CPumpCoreGlobalCtrl::GlobalCtrlWriteUnlock()
{
    s_csGlobalCtrl.writeUnlock();
}


}
}