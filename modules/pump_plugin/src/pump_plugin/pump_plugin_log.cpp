#include "pump_plugin/__pump_plugin_global_ctrl.h"
#include "pump_plugin/pump_plugin_log.h"

namespace Pump
{
namespace Plugin
{

/***********************************************
* __CPumpCoreLogGuide
***********************************************/
CPumpPluginLogGuide::CPumpPluginLogGuide()
{
}

CPumpPluginLogGuide::~CPumpPluginLogGuide()
{
}

pump_int32_t CPumpPluginLogGuide::WriteLine(
    PUMP_CORE_LOG_LEVEL emLevel,
    const char* szFile,
    unsigned int nLine,
    const char* szFormate,
    ...)
{
    std::string strMsg;
    ::Pump::Core::CLogData logData;
    logData.SetLevel(emLevel);
    //logData.SetSrcPos(szFile);
    va_list argv;
    va_start(argv, szFormate);
    logData.SetMessage(emLevel, szFile, nLine, szFormate, argv);
    va_end(argv);
    __CPumpPluginLogRecorderKeeper logKeeper = ::Pump::Plugin::__CPumpPluginGlobalCtrl::GetLogger();
    if (!logKeeper.GetPtr())
    {
        return PUMP_ERROR;
    }
    return logKeeper.GetPtr()->WriteLine(logData);
}

}
}
