/**
 * @file __pump_core_global_ctrl.h
 * @brief Library Inner Header.
 * @details Declaration of pump_core global resource control interface.
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

#ifndef __PUMP_CORE_GLOBAL_CTRL_H
#define __PUMP_CORE_GLOBAL_CTRL_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/pump_core_global_resource_mgr.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/logger/__pump_core_inner_logger.h"

namespace Pump
{
namespace Core
{

/**
 * @brief The global resource control interface in pump_core,
 * Singleton object. All functions this class should be called any
 * where inter pump_core, please use locker carefully.
 *
 * TODO All log recorder should allocate and store in CLogRecorderMgr object, and release 
 * by CLogRecorderMgr::~CLogRecorderMgr(), as well as m_pCmdSessionMgr.
 */
class __CPumpCoreGlobalCtrl
    : public CGlobalResourceMgr
{
public:
    /** Object initialize. */
    static pump_int32_t Init();

    /** Object cleanup. */
    static pump_int32_t Cleanup();

    /** Get pointer of global resource controler. */
    static __CPumpCoreGlobalCtrl * GetGlobalCtrl();

    /** Configure pump_core private logger. */
    static pump_int32_t SetLogger(const PUMP_CORE_LOG_CONF & struConf);

    /** Get pump_core private logger. */
    static CLogRecorderBase * GetLogger();

    /** Get logger manager. */
    static CLogRecorderMgr * GetLoggerMgr();

    /** Get CMD session manager. */
    static ::Pump::Core::Cmder::CCmdSessionMgr * GetCmdSessionMgr();
private:
    __CPumpCoreGlobalCtrl();
    virtual ~__CPumpCoreGlobalCtrl();
private:
    pump_bool_t m_bInit;
    ::Pump::Core::Thread::CMutex m_csInit;

    CLogRecorderMgr * m_pRecorderMgr; ///< Log recorder manager.
    ::Pump::Core::Thread::CMutex m_csRecorderMgr;

    CLogRecorderBase * m_pPumpCoreLogRecorder; ///< pump_core private log recorder.
    ::Pump::Core::Thread::CMutex m_csPumpCoreLogRecorder;

    ::Pump::Core::Cmder::CCmdSessionMgr * m_pCmdSessionMgr; ///< CMD session manager.
    ::Pump::Core::Thread::CMutex m_csCmdSessionMgr;
private:
    static __CPumpCoreGlobalCtrl * s_pGlobalCtrl; ///< Global resource instance.
    static ::Pump::Core::Thread::CMutex s_csGlobalCtrl; ///< Global resource instance locker.
};

}
}

#endif // __PUMP_CORE_GLOBAL_CTRL_H


