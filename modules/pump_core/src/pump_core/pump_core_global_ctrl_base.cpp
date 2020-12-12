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

#include <set>
#include "pump_core/pump_core_global_ctrl_base.h"
#include "pump_core/pump_core_object.h"
#include "pump_core/thread/pump_core_mutex.h"
#include "pump_core/logger/pump_core_logger.h"

namespace Pump
{
namespace Core
{


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

pump_bool_t CGlobalCtrlBase::GetInitFlag() const
{
    return m_bInit;
}

pump_int32_t CGlobalCtrlBase::SetLoggerPtr(pump_handle_t hLogger)
{
    CLogRecorderBase * pLogRecoreder = static_cast<CLogRecorderBase *>(hLogger);
    m_csLogRecorder.writeLock();
    m_pLogRecorder = pLogRecoreder;
    m_csLogRecorder.writeUnlock();
    return PUMP_OK;
}

::Pump::Core::CLogRecorderBase * CGlobalCtrlBase::GetLoggerPtr()
{
    return m_pLogRecorder;
}

void CGlobalCtrlBase::LogRecorderReadLock()
{
    m_csLogRecorder.readLock();
}

void CGlobalCtrlBase::LogRecorderReadUnlock()
{
    m_csLogRecorder.readUnlock();
}

}
}
