/**
 * @file pump_core_global_ctrl_base.h
 * @brief Library Header.
 * @details Declaration of global resource interface.
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

#ifndef PUMP_CORE_GLOBAL_CTRL_BASE_H
#define PUMP_CORE_GLOBAL_CTRL_BASE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_rwlocker.h"
#include "pump_core/pump_core_logger.h"

#ifdef PUMP_COMPILER_CXX
namespace Pump
{
namespace Core
{

class CLogRecorderBase;

class PUMP_CORE_CLASS CGlobalCtrlBase
  : public ::Pump::Core::CNonCopyOperator
{
public:
    CGlobalCtrlBase();
    virtual ~CGlobalCtrlBase();

protected:
    virtual pump_int32_t __Init() = 0;
    virtual pump_int32_t __Cleanup() = 0;

    pump_int32_t SetLoggerPtr(pump_handle_t hLogger);
    ::Pump::Core::CLogRecorderBase * GetLoggerPtr();
    void LogRecorderReadLock();
    void LogRecorderReadUnlock();
    void SetInitFlag(pump_bool_t bInit);
    pump_bool_t GetInitFlag() const;

private:
    pump_bool_t m_bInit;
    ::Pump::Core::Thread::CMutex m_csInit;

    ::Pump::Core::CLogRecorderBase * m_pLogRecorder; ///< private log recorder, [Not Owner]
    ::Pump::Core::Thread::CRWLocker m_csLogRecorder; ///< log recorder locker
};

}
}
#endif // PUMP_COMPILER_CXX

#endif // PUMP_CORE_GLOBAL_CTRL_BASE_H
