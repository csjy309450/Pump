#ifndef __PUMP_PLUGIN_GLOBAL_CTRL_H
#define __PUMP_PLUGIN_GLOBAL_CTRL_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_global_ctrl_base.h"
#include "pump_core/pump_core_global_resouce_keeper.hpp"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_plugin/pump_plugin_config.h"
#include "pump_plugin/pump_pslot.h"

namespace Pump
{
namespace Plugin
{

PUMP_PLUGIN_CXXAPI void PUMP_CALLBACK PUMP_PLUGIN_INNER_GlobalCtrlReadLock();

PUMP_PLUGIN_CXXAPI void PUMP_CALLBACK PUMP_PLUGIN_INNER_GlobalCtrlReadUnlock();

class __CPumpPluginGlobalCtrl;

template <class T>
class __CPumpPluginGlobalResouceKeeper
    : public ::Pump::Core::CGlobalResouceKeeperBase<T>
{
public:
    explicit __CPumpPluginGlobalResouceKeeper(T * ptr)
        : ::Pump::Core::CGlobalResouceKeeperBase<T>(ptr)
    {
        PUMP_PLUGIN_INNER_GlobalCtrlReadLock();
    }
    __CPumpPluginGlobalResouceKeeper(const __CPumpPluginGlobalResouceKeeper & other)
        : ::Pump::Core::CGlobalResouceKeeperBase<T>(other)
    {
        PUMP_PLUGIN_INNER_GlobalCtrlReadLock();
    }
    virtual ~__CPumpPluginGlobalResouceKeeper()
    {
        PUMP_PLUGIN_INNER_GlobalCtrlReadUnlock();
    }
};

class PUMP_PLUGIN_CLASS __CPumpPluginGlobalCtrlKeeper
    : public __CPumpPluginGlobalResouceKeeper<__CPumpPluginGlobalCtrl>
{
public:
    explicit __CPumpPluginGlobalCtrlKeeper(__CPumpPluginGlobalCtrl * pGlobalCtrl);
    ~__CPumpPluginGlobalCtrlKeeper();
    __CPumpPluginGlobalCtrlKeeper(__CPumpPluginGlobalCtrlKeeper & other);
};

class PUMP_PLUGIN_CLASS __CPumpPluginLogRecorderKeeper
    : public __CPumpPluginGlobalResouceKeeper<::Pump::Core::CLogRecorderBase>
{
public:
    explicit __CPumpPluginLogRecorderKeeper(::Pump::Core::CLogRecorderBase* pLogRecorder);
    __CPumpPluginLogRecorderKeeper(__CPumpPluginLogRecorderKeeper & other);
    virtual ~__CPumpPluginLogRecorderKeeper();
};

class PUMP_PLUGIN_CLASS __CPumpPluginGlobalCtrl
    : public ::Pump::Core::CGlobalCtrlBase
{
public:
    friend class __CPumpPluginLogRecorderKeeper;
    friend class __CPumpPluginGlobalCtrlKeeper;
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
    static __CPumpPluginGlobalCtrlKeeper GetGlobalCtrl();

    static void GlobalCtrlReadLock();
    static void GlobalCtrlReadUnlock();
    static void GlobalCtrlWriteLock();
    static void GlobalCtrlWriteUnlock();

    static pump_int32_t SetLogger(pump_handle_t hLogger);
    static __CPumpPluginLogRecorderKeeper GetLogger();

    static void SetPslotConfig(const PUMP_PSLOT_CONF & struConf);
    static PUMP_PSLOT_CONF GetPslotConfig();
private:
    __CPumpPluginGlobalCtrl();
    virtual ~__CPumpPluginGlobalCtrl();
    virtual pump_int32_t __Init();
    virtual pump_int32_t __Cleanup();
private:
    PUMP_PSLOT_CONF m_struConf;
    ::Pump::Core::Thread::CMutex m_csConf;

private:
    static __CPumpPluginGlobalCtrl * s_pGlobalCtrl; ///< Global resource instance.
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

#define __PUMP_PLUGIN_GLOBAL_CTRL() (::Pump::Plugin::__CPumpPluginGlobalCtrl::GetGlobalCtrl().GetPtr())

#endif //__PUMP_PLUGIN_GLOBAL_CTRL_H


