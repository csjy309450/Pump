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
#include "pump_core/pump_core_global_ctrl_base.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/logger/__pump_core_inner_logger.h"

namespace Pump
{
namespace Core
{

class __CPumpCoreGlobalCtrl;

class PUMP_CORE_CLASS __CPumpCoreGlobalCtrlGuider
{
public:
    explicit __CPumpCoreGlobalCtrlGuider(__CPumpCoreGlobalCtrl * pLogRecorder);
    ~__CPumpCoreGlobalCtrlGuider();
    __CPumpCoreGlobalCtrlGuider(__CPumpCoreGlobalCtrlGuider & other);
    __CPumpCoreGlobalCtrl * GetPumpCoreGlobalCtrl();
private:
    __CPumpCoreGlobalCtrlGuider();
private:
    __CPumpCoreGlobalCtrl * m_pPumpCoreGlobalCtrl;
};

/**
 * @brief The global resource control interface in pump_core,
 * Singleton object. All functions this class should be called any
 * where inter pump_core, please use locker carefully.
 *
 * TODO All log recorder should allocate and store in CLogRecorderMgr object, and release 
 * by CLogRecorderMgr::~CLogRecorderMgr(), as well as m_pCmdSessionMgr.
 */
class PUMP_CORE_CLASS __CPumpCoreGlobalCtrl
    : public CGlobalCtrlBase
{
public:
    friend class __CPumpCoreGlobalCtrlGuider;
public:
    /** Object Create. */
    static pump_int32_t Create();

    /** Object Destroy. */
    static pump_int32_t Destroy();
    /** Object initialize. */
    static pump_int32_t Init();

    /** Object cleanup. */
    static pump_int32_t Cleanup();

    /** Get pointer of global resource controler. */
    static __CPumpCoreGlobalCtrlGuider GetGlobalCtrl();

    /** Get logger manager. */
    static CLogRecorderMgr * GetLoggerMgr();

    /** Get CMD session manager. */
    static ::Pump::Core::Cmder::CCmdSessionMgr * GetCmdSessionMgr();
private:
    __CPumpCoreGlobalCtrl();
    virtual ~__CPumpCoreGlobalCtrl();
    pump_int32_t __Init();
    pump_int32_t __Cleanup();
private:
    //pump_bool_t m_bInit;
    //::Pump::Core::Thread::CMutex m_csInit;

    CLogRecorderMgr * m_pRecorderMgr; ///< Log recorder manager.
    ::Pump::Core::Thread::CMutex m_csRecorderMgr;

    //CLogRecorderBase * m_pPumpCoreLogRecorder; ///< pump_core private log recorder.
    //::Pump::Core::Thread::CMutex m_csPumpCoreLogRecorder;

    ::Pump::Core::Cmder::CCmdSessionMgr * m_pCmdSessionMgr; ///< CMD session manager.
    ::Pump::Core::Thread::CMutex m_csCmdSessionMgr;
//private:
//    static __CPumpCoreGlobalCtrl * s_pGlobalCtrl; ///< Global resource instance.
//    static ::Pump::Core::Thread::CMutex s_csGlobalCtrl; ///< Global resource instance locker.
};

#define __PUMP_CORE_GLOBAL_CTRL2() (static_cast<__CPumpCoreGlobalCtrl *>(__CPumpCoreGlobalCtrl::s_pGlobalCtrl))

}
}

#define __PUMP_CORE_GLOBAL_CTRL() (::Pump::Core::__CPumpCoreGlobalCtrl::GetGlobalCtrl().GetPumpCoreGlobalCtrl())

#endif // __PUMP_CORE_GLOBAL_CTRL_H


