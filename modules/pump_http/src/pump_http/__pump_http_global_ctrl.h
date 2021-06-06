#ifndef __PUMP_HTTP_GLOBAL_CTRL_H
#define __PUMP_HTTP_GLOBAL_CTRL_H

#include "pump_macro/pump_pre.h"
#include "pump_core/os_wrapper/pump_core_os_types.h"
#include "pump_core/pump_core_mutex.h"
#include "pump_core/pump_core_global_ctrl_base.h"

namespace Pump
{
namespace Http
{

class __CPumpHttpGlobalCtrl 
    : ::Pump::Core::CGlobalCtrlBase
{
public:
    __CPumpHttpGlobalCtrl();
    virtual ~__CPumpHttpGlobalCtrl();
    static pump_int32_t Init();
    static pump_int32_t Cleanup();
    static __CPumpHttpGlobalCtrl * GetGlobalCtrlLocked();
    static void GlobalCtrlUnlocked();
    pump_int32_t CreateWorkingThxPool();
    pump_int32_t DestroyWorkingThxPool();
    pump_handle_t GetWorkingThxPoolLocked();
    void WorkingThxPoolUnlock();
private:
    virtual pump_int32_t __Init();
    virtual pump_int32_t __Cleanup();
private:
    pump_handle_t m_hWorkingThxPool;
    ::Pump::Core::Thread::CMutex m_csWorkingThxPool;
private:
    static __CPumpHttpGlobalCtrl * s_pGlobalCtrl;
    static ::Pump::Core::Thread::CMutex s_csGlobalCtrl;
};

}
}

#endif // __PUMP_HTTP_GLOBAL_CTRL_H