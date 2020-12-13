#include "pump_plugin/__pump_plugin_global_ctrl.h"

namespace Pump
{
namespace Plugin
{
#define __PUMP_PLUGIN_GLOBAL_CTRL2() (__CPumpPluginGlobalCtrl::s_pGlobalCtrl)

__CPumpPluginGlobalCtrl * __CPumpPluginGlobalCtrl::s_pGlobalCtrl = NULL;
::Pump::Core::Thread::CRWLocker __CPumpPluginGlobalCtrl::s_csGlobalCtrl;

PUMP_PLUGIN_CXXAPI void PUMP_CALLBACK PUMP_PLUGIN_INNER_GlobalCtrlReadLock()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
}

PUMP_PLUGIN_CXXAPI void PUMP_CALLBACK PUMP_PLUGIN_INNER_GlobalCtrlReadUnlock()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
}

/***********************************************
* __CPumpPluginGlobalCtrlKeeper
***********************************************/
__CPumpPluginGlobalCtrlKeeper::__CPumpPluginGlobalCtrlKeeper(__CPumpPluginGlobalCtrl * pGlobalCtrl)
    : __CPumpPluginGlobalResouceKeeper<__CPumpPluginGlobalCtrl>(pGlobalCtrl)
{}

__CPumpPluginGlobalCtrlKeeper::~__CPumpPluginGlobalCtrlKeeper()
{}

__CPumpPluginGlobalCtrlKeeper::__CPumpPluginGlobalCtrlKeeper(__CPumpPluginGlobalCtrlKeeper & other)
    : __CPumpPluginGlobalResouceKeeper<__CPumpPluginGlobalCtrl>(other)
{}

/***********************************************
* __CPumpPluginLogRecorderKeeper
***********************************************/
__CPumpPluginLogRecorderKeeper::__CPumpPluginLogRecorderKeeper(::Pump::Core::CLogRecorderBase* pLogRecorder)
    : __CPumpPluginGlobalResouceKeeper<::Pump::Core::CLogRecorderBase>(pLogRecorder)
{
    if (!__PUMP_PLUGIN_GLOBAL_CTRL2() || !pLogRecorder)
    {
        return;
    }
    __PUMP_PLUGIN_GLOBAL_CTRL2()->LogRecorderReadLock();
}

__CPumpPluginLogRecorderKeeper::__CPumpPluginLogRecorderKeeper(__CPumpPluginLogRecorderKeeper & other)
    : __CPumpPluginGlobalResouceKeeper<::Pump::Core::CLogRecorderBase>(other)
{
    if (!__PUMP_PLUGIN_GLOBAL_CTRL2() || !other.GetPtr())
    {
        return;
    }
    __PUMP_PLUGIN_GLOBAL_CTRL2()->LogRecorderReadLock();
}

__CPumpPluginLogRecorderKeeper::~__CPumpPluginLogRecorderKeeper()
{
    if (!__PUMP_PLUGIN_GLOBAL_CTRL2() || !this->GetPtr())
    {
        return;
    }
    __PUMP_PLUGIN_GLOBAL_CTRL2()->LogRecorderReadUnlock();
}

/***********************************************
* __CPumpPluginGlobalCtrl
***********************************************/
__CPumpPluginGlobalCtrl::__CPumpPluginGlobalCtrl()
    : CGlobalCtrlBase()
{
    memset(&m_struConf, 0, sizeof(m_struConf));
}

__CPumpPluginGlobalCtrl::~__CPumpPluginGlobalCtrl()
{
    this->__Cleanup();
}

pump_int32_t __CPumpPluginGlobalCtrl::__Init()
{
    /** Set init flag. */
    this->SetInitFlag(PUMP_TRUE);
    return PUMP_OK;
}

pump_int32_t __CPumpPluginGlobalCtrl::__Cleanup()
{
    this->SetInitFlag(PUMP_FALSE);
    return PUMP_OK;
}

/** Object Create. */
pump_int32_t __CPumpPluginGlobalCtrl::Create()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_OK;
    }
    // double protection lock, 
    if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        __CPumpPluginGlobalCtrl::GlobalCtrlWriteLock();
        if (__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock();
            return PUMP_OK;
        }
        else
        {
            __CPumpPluginGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) __CPumpPluginGlobalCtrl();
            __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock();
            if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
            {
                return PUMP_ERROR;
            }
        }
    }
    return PUMP_OK;
}

/** Object Destroy. */
pump_int32_t __CPumpPluginGlobalCtrl::Destroy()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_OK;
    }
    // double protection lock, 
    if (__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        __CPumpPluginGlobalCtrl::GlobalCtrlWriteLock();
        if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock();
            return PUMP_OK;
        }
        else
        {
            delete __CPumpPluginGlobalCtrl::s_pGlobalCtrl;
            __CPumpPluginGlobalCtrl::s_pGlobalCtrl = NULL;
            __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock();
            if (__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
            {
                return PUMP_ERROR;
            }
        }
    }
    return PUMP_OK;
}

pump_int32_t __CPumpPluginGlobalCtrl::Init()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (__CPumpPluginGlobalCtrl::IsInit())
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_OK;
    }

    int ret = __PUMP_PLUGIN_GLOBAL_CTRL2()->__Init();
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
    return ret;
}

pump_bool_t __CPumpPluginGlobalCtrl::IsInit()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (!s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_FALSE;
    }
    pump_bool_t bInit = s_pGlobalCtrl->GetInitFlag();
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
    return bInit;
}

pump_int32_t __CPumpPluginGlobalCtrl::Cleanup()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (!__PUMP_PLUGIN_GLOBAL_CTRL2()->IsInit())
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_ERROR;
    }
    __PUMP_PLUGIN_GLOBAL_CTRL2()->__Cleanup();
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
    return PUMP_OK;
}

__CPumpPluginGlobalCtrlKeeper __CPumpPluginGlobalCtrl::GetGlobalCtrl()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (__CPumpPluginGlobalCtrl::s_pGlobalCtrl && __CPumpPluginGlobalCtrl::s_pGlobalCtrl->IsInit())
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return __CPumpPluginGlobalCtrlKeeper(__PUMP_PLUGIN_GLOBAL_CTRL2());
    }
    // double protection lock, 
    if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlWriteLock();
        if (__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
        {
            __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock();
            __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
            return __CPumpPluginGlobalCtrlKeeper(__PUMP_PLUGIN_GLOBAL_CTRL2());
        }
        else
        {
            __CPumpPluginGlobalCtrl::s_pGlobalCtrl = new (std::nothrow) __CPumpPluginGlobalCtrl();
            __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock();
            if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl)
            {
                __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
                return __CPumpPluginGlobalCtrlKeeper(NULL);
            }
            __PUMP_PLUGIN_GLOBAL_CTRL2()->Init();
        }
    }
    if (!__CPumpPluginGlobalCtrl::s_pGlobalCtrl->IsInit())
    {
        __PUMP_PLUGIN_GLOBAL_CTRL2()->Init();
    }
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
    return __CPumpPluginGlobalCtrlKeeper(__PUMP_PLUGIN_GLOBAL_CTRL2());
}

void __CPumpPluginGlobalCtrl::GlobalCtrlReadLock()
{
    s_csGlobalCtrl.readLock();
}

void __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock()
{
    s_csGlobalCtrl.readUnlock();
}

void __CPumpPluginGlobalCtrl::GlobalCtrlWriteLock()
{
    s_csGlobalCtrl.writeLock();
}

void __CPumpPluginGlobalCtrl::GlobalCtrlWriteUnlock()
{
    s_csGlobalCtrl.writeUnlock();
}

pump_int32_t __CPumpPluginGlobalCtrl::SetLogger(pump_handle_t hLogger)
{
    if (!hLogger)
    {
        return PUMP_ERROR;
    }
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (!s_pGlobalCtrl)
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
        return PUMP_OK;
    }
    s_pGlobalCtrl->SetLoggerPtr(hLogger);

    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
    return PUMP_OK;
}

__CPumpPluginLogRecorderKeeper __CPumpPluginGlobalCtrl::GetLogger()
{
    return __CPumpPluginLogRecorderKeeper((::Pump::Core::CLogRecorderBase*)(!s_pGlobalCtrl ? NULL : s_pGlobalCtrl->GetLoggerPtr()));
}

void __CPumpPluginGlobalCtrl::SetPslotConfig(const PUMP_PSLOT_CONF & struConf)
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (!__CPumpPluginGlobalCtrl::IsInit())
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return;
    }
    __PUMP_PLUGIN_GLOBAL_CTRL2()->m_csConf.Lock();
    __PUMP_PLUGIN_GLOBAL_CTRL2()->m_struConf = struConf;
    __PUMP_PLUGIN_GLOBAL_CTRL2()->m_csConf.Unlock();
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
}

PUMP_PSLOT_CONF __CPumpPluginGlobalCtrl::GetPslotConfig()
{
    __CPumpPluginGlobalCtrl::GlobalCtrlReadLock();
    if (!__CPumpPluginGlobalCtrl::IsInit())
    {
        __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
        return PUMP_PSLOT_CONF(); // FIXME 
    }
    PUMP_PSLOT_CONF struCpy = __PUMP_PLUGIN_GLOBAL_CTRL2()->m_struConf;
    __CPumpPluginGlobalCtrl::GlobalCtrlReadUnlock();
    return struCpy;
}

}
}
