#include "pump_core/pump_core_app.h"
#include "pump_core/__pump_core_global_ctrl.h"

namespace Pump
{
namespace Core
{

::Pump::Core::Thread::CMutex CApplication::s_csCount;
pump_int32_t CApplication::s_nCount = 0;

CApplication::CApplication()
    //: CNonNewOperator("CApplication", PUMP_FALSE)
    //, CNonCopyOperator("CApplication", PUMP_FALSE)
{
    s_csCount.Lock();
    if (s_nCount > 0)
    {
        s_nCount++;
        s_csCount.Unlock();
        return;
    }
    if (::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        s_csCount.Unlock();
        return;
    }
    if (::Pump::Core::__CPumpCoreGlobalCtrl::Create() != PUMP_OK)
    {
        s_csCount.Unlock();
        return;
    }
    if (::Pump::Core::__CPumpCoreGlobalCtrl::Init() != PUMP_OK)
    {
        ::Pump::Core::__CPumpCoreGlobalCtrl::Destroy();
        s_csCount.Unlock();
        return;
    }
    s_nCount++;
    s_csCount.Unlock();
}

CApplication::~CApplication()
{
    s_csCount.Lock();
    if (s_nCount > 1)
    {
        s_nCount--;
        s_csCount.Unlock();
        return;
    }
    if (!::Pump::Core::__CPumpCoreGlobalCtrl::IsInit())
    {
        s_csCount.Unlock();
        return;
    }
    if (::Pump::Core::__CPumpCoreGlobalCtrl::Cleanup() != PUMP_OK)
    {
        s_csCount.Unlock();
        return;
    }
    ::Pump::Core::__CPumpCoreGlobalCtrl::Destroy();
    s_nCount--;
    s_csCount.Unlock();
}

pump_bool_t CApplication::IsInit()
{
    return (s_nCount > 0 ? PUMP_TRUE : PUMP_FALSE);
}

pump_int32_t SetLogger(CLogRecorderBase * plogger)
{
    return ::Pump::Core::__CPumpCoreGlobalCtrl::SetLogger((pump_handle_t)plogger);
}

}
}