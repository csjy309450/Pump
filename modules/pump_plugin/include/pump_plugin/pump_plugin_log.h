#ifndef PUMP_PLUGIN_LOG_H
#define PUMP_PLUGIN_LOG_H
#include "pump_plugin/pump_plugin_config.h"
#include "pump_core/pump_core_logger.h"

#ifdef PUMP_COMPILER_CXX
#define PUMP_PLUGIN_INFO(form_, ...) ::Pump::Plugin::CPumpPluginLogGuide().WriteLine(PUMP_LOG_INFO, __FILE__,__LINE__, form_,__VA_ARGS__)
#define PUMP_PLUGIN_WARING(form_, ...) ::Pump::Plugin::CPumpPluginLogGuide().WriteLine(PUMP_LOG_WARNING, __FILE__,__LINE__, form_,__VA_ARGS__)
#define PUMP_PLUGIN_ERROR(form_, ...) ::Pump::Plugin::CPumpPluginLogGuide().WriteLine(PUMP_LOG_ERROR, __FILE__,__LINE__, form_,__VA_ARGS__)
#define PUMP_PLUGIN_LOG_ASSERT(exp_,form_, ...) assert(exp_);PUMP_PLUGIN_INFO(form_, __VA_ARGS__)
#else
#define PUMP_PLUGIN_INFO(form_, ...) 
#define PUMP_PLUGIN_WARING(form_, ...) 
#define PUMP_PLUGIN_ERROR(form_, ...) 
#define PUMP_PLUGIN_LOG_ASSERT(exp_,form_, ...)
#endif // PUMP_COMPILER_CXX

namespace Pump
{
namespace Plugin
{

class PUMP_PLUGIN_CLASS CPumpPluginLogGuide
    : public ::Pump::Core::CLogGuideBase
{
public:
    CPumpPluginLogGuide();
    virtual ~CPumpPluginLogGuide();

public:
    virtual pump_int32_t WriteLine(
        PUMP_CORE_LOG_LEVEL emLevel,
        const char* szFile,
        unsigned int nLine,
        const char* szFormate,
        ...);
};

}
}

#endif //__PUMP_PLUGIN_LOG_H