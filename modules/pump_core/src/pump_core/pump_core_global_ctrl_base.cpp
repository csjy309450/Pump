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

//CLogRecorderGuider::CLogRecorderGuider(CLogRecorderBase * pLogRecorder)
//    : m_pLogRecorder(pLogRecorder)
//{
//    CGlobalCtrlBase::ReadLock();
//}
//
//CLogRecorderGuider::CLogRecorderGuider(const CLogRecorderGuider & other)
//    : m_pLogRecorder(other.m_pLogRecorder)
//{
//    CGlobalCtrlBase::ReadLock();
//}
//
//CLogRecorderGuider::~CLogRecorderGuider() 
//{ 
//    m_pLogRecorder = NULL;
//    CGlobalCtrlBase::ReadUnlock();
//}
//CLogRecorderBase * CLogRecorderGuider::GetLogger() { return m_pLogRecorder; }

//CLogRecorderGuider::CLogRecorderGuider() {}

CGlobalCtrlBase * CGlobalCtrlBase::s_pGlobalCtrl = PUMP_NULL;
::Pump::Core::Thread::CRWLocker CGlobalCtrlBase::s_rcsGlobalCtrl;
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
    s_rcsGlobalCtrl.readLock();
}

void CGlobalCtrlBase::ReadUnlock()
{
    s_rcsGlobalCtrl.readUnlock();
}

void CGlobalCtrlBase::WriteLock()
{
    s_rcsGlobalCtrl.writeLock();
}

void CGlobalCtrlBase::WriteUnlock()
{
    s_rcsGlobalCtrl.writeUnlock();
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
        return PUMP_FALSE;
    }
    CLogRecorderBase * pLogRecoreder = static_cast<CLogRecorderBase *>(hLogger);
    s_pGlobalCtrl->m_csLogRecorder.Lock();
    s_pGlobalCtrl->m_pLogRecorder = pLogRecoreder;
    s_pGlobalCtrl->m_csLogRecorder.Unlock();
    CGlobalCtrlBase::ReadUnlock();
    return PUMP_ERROR;
}

/** Get private logger. */
CLogRecorderGuider CGlobalCtrlBase::GetLogger()
{
    return CLogRecorderGuider((CLogRecorderBase*)(!s_pGlobalCtrl ? NULL : s_pGlobalCtrl->m_pLogRecorder));
}

}
}
