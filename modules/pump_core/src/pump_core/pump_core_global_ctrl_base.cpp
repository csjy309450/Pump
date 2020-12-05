/**
 * @file pump_core_global_ctrl_base.cpp
 * @brief Library Header.
 * @details Implementation of global resource interface.
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

#include "pump_core/pump_core_global_ctrl_base.h"
#include "pump_core/pump_core_object.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/logger/pump_core_logger.h"

namespace Pump
{
namespace Core
{

//CLogRecorderKeeper::CLogRecorderKeeper(CLogRecorderBase * pLogRecorder)
//    : m_pLogRecorder(pLogRecorder)
//{
//    CGlobalCtrlBase::ReadLock();
//}
//
//CLogRecorderKeeper::CLogRecorderKeeper(const CLogRecorderKeeper & other)
//    : m_pLogRecorder(other.m_pLogRecorder)
//{
//    CGlobalCtrlBase::ReadLock();
//}
//
//CLogRecorderKeeper::~CLogRecorderKeeper() 
//{ 
//    m_pLogRecorder = NULL;
//    CGlobalCtrlBase::ReadUnlock();
//}
//CLogRecorderBase * CLogRecorderKeeper::GetLogger() { return m_pLogRecorder; }

//CLogRecorderKeeper::CLogRecorderKeeper() {}

CGlobalCtrlBase * CGlobalCtrlBase::s_pGlobalCtrl = PUMP_NULL;
::Pump::Core::Thread::CRWLocker CGlobalCtrlBase::s_csGlobalCtrl;
//::Pump::Core::Thread::CMutex CGlobalCtrlBase::s_wcsGlobalCtrl;

CGlobalCtrlBase::CGlobalCtrlBase()
    : m_bInit(PUMP_FALSE)
    , m_csInit()
    , m_pLogRecorder(PUMP_NULL)
    , m_csLogRecorder()
{
}

CGlobalCtrlBase::~CGlobalCtrlBase()
{
    m_bInit = PUMP_FALSE;
    m_pLogRecorder = (PUMP_NULL);
}

void CGlobalCtrlBase::ReadLock()
{
    s_csGlobalCtrl.readLock();
}

void CGlobalCtrlBase::ReadUnlock()
{
    s_csGlobalCtrl.readUnlock();
}

void CGlobalCtrlBase::WriteLock()
{
    s_csGlobalCtrl.writeLock();
}

void CGlobalCtrlBase::WriteUnlock()
{
    s_csGlobalCtrl.writeUnlock();
}

void CGlobalCtrlBase::SetInitFlag(pump_bool_t bInit)
{
    if (bInit == m_bInit)
    {
        return;
    }
    m_csInit.Lock();
    if (bInit != m_bInit)
    {
        m_bInit = bInit;
    }
    m_csInit.Unlock();
}

pump_bool_t CGlobalCtrlBase::IsInit()
{
    CGlobalCtrlBase::ReadLock();
    if (!s_pGlobalCtrl)
    {
        CGlobalCtrlBase::ReadUnlock();
        return PUMP_FALSE;
    }
    pump_bool_t bInit = s_pGlobalCtrl->m_bInit;
    CGlobalCtrlBase::ReadUnlock();
    return bInit;
}

/** Set private logger. */
pump_int32_t CGlobalCtrlBase::SetLogger(pump_handle_t hLogger)
{
    if (!hLogger)
    {
        return PUMP_ERROR;
    }
    CGlobalCtrlBase::ReadLock();
    if (!s_pGlobalCtrl)
    {
        CGlobalCtrlBase::ReadLock();
        return PUMP_OK;
    }
    CLogRecorderBase * pLogRecoreder = static_cast<CLogRecorderBase *>(hLogger);
    s_pGlobalCtrl->m_csLogRecorder.writeLock();
    s_pGlobalCtrl->m_pLogRecorder = pLogRecoreder;
    s_pGlobalCtrl->m_csLogRecorder.writeUnlock();
    CGlobalCtrlBase::ReadUnlock();
    return PUMP_OK;
}

/** Get private logger. */
CLogRecorderKeeper CGlobalCtrlBase::GetLogger()
{
    return CLogRecorderKeeper((CLogRecorderBase*)(!s_pGlobalCtrl ? NULL : s_pGlobalCtrl->m_pLogRecorder));
}

}
}
