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
::Pump::Core::Thread::CMutex __CPumpCoreGlobalCtrl::s_csGlobalCtrl;

__CPumpCoreGlobalCtrl::__CPumpCoreGlobalCtrl()
    : m_bInit(PUMP_FALSE)
    , m_pRecorderMgr(PUMP_NULL)
    , m_pPumpCoreLogRecorder(PUMP_NULL)
    , m_pCmdSessionMgr(PUMP_NULL)
{
}

__CPumpCoreGlobalCtrl::~__CPumpCoreGlobalCtrl()
{
    this->Cleanup();
}

pump_int32_t __CPumpCoreGlobalCtrl::Init()
{
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_bInit)
    {
        return PUMP_OK;
    }

    /** Initialize log recorder manger which must init before m_pPumpCoreLogRecorder*/
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr)
    {
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csRecorderMgr.Lock();
        if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr)
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr
                = new (std::nothrow) ::Pump::Core::CLogRecorderMgr();
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csRecorderMgr.Unlock();
            if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr)
            {
                __CPumpCoreGlobalCtrl::s_pGlobalCtrl->Cleanup();
                return PUMP_ERROR;
            }
        }
    }

    /** Initialize pump_core private log recorder */
    //if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder)
    //{
    //    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csPumpCoreLogRecorder.Lock();
    //    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder)
    //    {
    //        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder
    //            = __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr->Create(
    //            PUMP_LOG_RECORED_DEFAULT);
    //        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csPumpCoreLogRecorder.Unlock();
    //        if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder)
    //        {
    //            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->Cleanup();
    //            return PUMP_ERROR;
    //        }
    //    }
    //}

    /** Initialize CMD session manger. */
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr)
    {
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csCmdSessionMgr.Lock();
        if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr)
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr
                = new  (std::nothrow) ::Pump::Core::Cmder::CCmdSessionMgr();
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csCmdSessionMgr.Unlock();
            if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr)
            {
                __CPumpCoreGlobalCtrl::s_pGlobalCtrl->Cleanup();
                return PUMP_ERROR;
            }
        }
    }

    /** Set init flag. */
    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csInit.Lock();
    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_bInit = PUMP_TRUE;
    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csInit.Unlock();
    return PUMP_OK;
}

pump_bool_t __CPumpCoreGlobalCtrl::IsInit()
{
    return __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_bInit;
}

pump_int32_t __CPumpCoreGlobalCtrl::Cleanup()
{
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_bInit)
    {
        return PUMP_ERROR;
    }
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr)
    {
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csCmdSessionMgr.Lock();
        delete __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr = NULL;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csCmdSessionMgr.Unlock();
    }
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder)
    {
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csPumpCoreLogRecorder.Lock();
        delete __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder = NULL;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csPumpCoreLogRecorder.Unlock();
    }
    if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr)
    {
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csRecorderMgr.Lock();
        delete __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr = NULL;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csRecorderMgr.Unlock();
    }
    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csInit.Lock();
    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_bInit = PUMP_FALSE;
    __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csInit.Unlock();
    return PUMP_OK;
}

__CPumpCoreGlobalCtrl * __CPumpCoreGlobalCtrl::GetGlobalCtrl()
{
    // double protection lock, 
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpCoreGlobalCtrl::s_csGlobalCtrl.Lock();
        if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpCoreGlobalCtrl::s_csGlobalCtrl.Unlock();
            return __CPumpCoreGlobalCtrl::s_pGlobalCtrl;
        }
        else
        {
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) ::Pump::Core::__CPumpCoreGlobalCtrl();
            __CPumpCoreGlobalCtrl::s_csGlobalCtrl.Unlock();
            if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl)
            {
                return NULL;
            }
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->Init();
        }
    }
    if (!__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_bInit)
    {
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->Init();
    }
    return __CPumpCoreGlobalCtrl::s_pGlobalCtrl;
}

CLogRecorderBase * __CPumpCoreGlobalCtrl::GetLogger()
{
    if (__CPumpCoreGlobalCtrl::GetGlobalCtrl())
    {
        return __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder;
    }
    return NULL;
}

CLogRecorderMgr * __CPumpCoreGlobalCtrl::GetLoggerMgr()
{
    if (__CPumpCoreGlobalCtrl::GetGlobalCtrl())
    {
        return __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pRecorderMgr;
    }
    return NULL;
}

::Pump::Core::Cmder::CCmdSessionMgr * __CPumpCoreGlobalCtrl::GetCmdSessionMgr()
{
    if (__CPumpCoreGlobalCtrl::GetGlobalCtrl())
    {
        return __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pCmdSessionMgr;
    }
    return NULL;
}

pump_int32_t __CPumpCoreGlobalCtrl::SetLogger(pump_handle_t hLogger)
{
    if (!hLogger)
    {
        return PUMP_ERROR;
    }
    if (__CPumpCoreGlobalCtrl::GetGlobalCtrl())
    {
        CLogRecorderBase * pLogRecoreder = static_cast<CLogRecorderBase *>(hLogger);
        // Check pLogRecoreder available.
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csPumpCoreLogRecorder.Lock();
        if (__CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder)
        {
            delete __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder;
            __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder = NULL;
        }
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_pPumpCoreLogRecorder = pLogRecoreder;
        __CPumpCoreGlobalCtrl::s_pGlobalCtrl->m_csPumpCoreLogRecorder.Unlock();
    }
    return PUMP_ERROR;
}

}
}