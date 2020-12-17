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
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_global_ctrl_base.h"
#include "pump_core/pump_core_cmder.h"
#include "pump_core/pump_core_global_resouce_keeper.hpp"
#include "pump_core/pump_core_logger.h"
#include "pump_core/__pump_core_inner_logger.h"

namespace Pump
{
namespace Core
{

class __CPumpCoreGlobalCtrl;

PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadLock();
PUMP_CORE_CXXAPI void PUMP_CALLBACK PUMP_CORE_INNER_GlobalCtrlReadUnlock();

template <class T>
class __CPumpCoreGlobalResouceKeeper
    : public CGlobalResouceKeeperBase<T>
{
public:
    explicit __CPumpCoreGlobalResouceKeeper(T * ptr)
        : CGlobalResouceKeeperBase<T>(ptr)
    {
        PUMP_CORE_INNER_GlobalCtrlReadLock();
    }
    __CPumpCoreGlobalResouceKeeper(const __CPumpCoreGlobalResouceKeeper & other)
        : CGlobalResouceKeeperBase<T>(other)
    {
        PUMP_CORE_INNER_GlobalCtrlReadLock();
    }
    virtual ~__CPumpCoreGlobalResouceKeeper()
    {
        PUMP_CORE_INNER_GlobalCtrlReadUnlock();
    }
};

class PUMP_CORE_CLASS __CPumpCoreGlobalCtrlKeeper
    : public __CPumpCoreGlobalResouceKeeper<__CPumpCoreGlobalCtrl>
{
public:
    explicit __CPumpCoreGlobalCtrlKeeper(__CPumpCoreGlobalCtrl * pGlobalCtrl);
    ~__CPumpCoreGlobalCtrlKeeper();
    __CPumpCoreGlobalCtrlKeeper(__CPumpCoreGlobalCtrlKeeper & other);
};

class PUMP_CORE_CLASS __CPumpCoreLogRecorderKeeper
    : public __CPumpCoreGlobalResouceKeeper<CLogRecorderBase>
{
public:
    explicit __CPumpCoreLogRecorderKeeper(CLogRecorderBase* pLogRecorder);
    __CPumpCoreLogRecorderKeeper(__CPumpCoreLogRecorderKeeper & other);
    virtual ~__CPumpCoreLogRecorderKeeper();
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
    friend class __CPumpCoreLogRecorderKeeper;
    friend class __CPumpCoreGlobalCtrlKeeper;
public:
    /** Object Create. */
    static pump_int32_t Create();

    /** Object Destroy. */
    static pump_int32_t Destroy();

    /** Object initialize. */
    static pump_int32_t Init();

    static pump_bool_t IsInit();

    /** Object cleanup. */
    static pump_int32_t Cleanup();

    /** Get pointer of global resource controler. */
    static __CPumpCoreGlobalCtrlKeeper GetGlobalCtrl();

    /** Get logger manager.
     * [FXIME] refer to __CPumpCoreLogRecorderKeeper
     */
    static CLogRecorderMgr * GetLoggerMgr();

    /** CMD session manager. 
     * [FXIME] refer to __CPumpCoreLogRecorderKeeperGet 
     */
    static ::Pump::Core::Cmder::CCmdSessionMgr * GetCmdSessionMgr();

    static void GlobalCtrlReadLock();
    static void GlobalCtrlReadUnlock();
    static void GlobalCtrlWriteLock();
    static void GlobalCtrlWriteUnlock();

    static pump_int32_t SetLogger(pump_handle_t hLogger);
    static __CPumpCoreLogRecorderKeeper GetLogger();
private:
    __CPumpCoreGlobalCtrl();
    virtual ~__CPumpCoreGlobalCtrl();
    virtual pump_int32_t __Init();
    virtual pump_int32_t __Cleanup();
private:
    CLogRecorderMgr * m_pRecorderMgr; ///< Log recorder manager.
    ::Pump::Core::Thread::CMutex m_csRecorderMgr;

    ::Pump::Core::Cmder::CCmdSessionMgr * m_pCmdSessionMgr; ///< CMD session manager.
    ::Pump::Core::Thread::CMutex m_csCmdSessionMgr;
private:
    static __CPumpCoreGlobalCtrl * s_pGlobalCtrl; ///< Global resource instance.
    /**
     * Global instance RW locker. <b>s_csGlobalCtrl<\b> prevent to free s_pGlobalCtrl \
     * while other threads reference s_pGlobalCtrl.
     * - When using s_pGlobalCtrl on read locker.
     * - When assign/delete s_pGlobalCtrl on write locker.
     */
    static ::Pump::Core::Thread::CRWLocker s_csGlobalCtrl;
};

}
}

#define __PUMP_CORE_GLOBAL_CTRL() (::Pump::Core::__CPumpCoreGlobalCtrl::GetGlobalCtrl().GetPtr())

#endif // __PUMP_CORE_GLOBAL_CTRL_H


