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
#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/thread/pump_core_rwlocker.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_global_resouce_guider.hpp"

#ifdef PUMP_COMPILER_CXX
namespace Pump
{
namespace Core
{

class CLogRecorderBase;

class PUMP_CORE_CLASS CGlobalCtrlBase
  : public ::Pump::Core::CNonCopyable
{
public:
    friend class CLogRecorderKeeper;
public:
    CGlobalCtrlBase();
    virtual ~CGlobalCtrlBase();

    static pump_bool_t IsInit();
    static pump_int32_t SetLogger(pump_handle_t hLogger);
    static CLogRecorderKeeper GetLogger();
    static void ReadLock();
    static void ReadUnlock();
    static void WriteLock();
    static void WriteUnlock();

protected:
    void SetInitFlag(pump_bool_t bInit);

private:
    pump_bool_t m_bInit;
    ::Pump::Core::Thread::CMutex m_csInit;

    ::Pump::Core::CLogRecorderBase * m_pLogRecorder; ///< private log recorder, [Not Owner]
    ::Pump::Core::Thread::CRWLocker m_csLogRecorder; ///< log recorder locker

protected:
    static CGlobalCtrlBase * s_pGlobalCtrl; ///< Global resource instance.
    /**
     * Global instance RW locker. <b>s_csGlobalCtrl<\b> prevent to free s_pGlobalCtrl \
     * while other threads reference s_pGlobalCtrl.
     * - When using s_pGlobalCtrl on read locker.
     * - When assign/delete s_pGlobalCtrl on write locker.
     */
    static ::Pump::Core::Thread::CRWLocker s_csGlobalCtrl;
    //static ::Pump::Core::Thread::CMutex s_wcsGlobalCtrl; // FIXME 需要改成递归锁
};

}
}
#endif // PUMP_COMPILER_CXX

#endif // PUMP_CORE_GLOBAL_CTRL_BASE_H
