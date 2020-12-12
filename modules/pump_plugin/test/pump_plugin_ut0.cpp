#include <iostream>
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/logger/pump_core_logger.h"
#include "pump_plugin/pump_pslot.h"
#include "pump_plugin/pump_plugin_log.h"
#include "pump_plugin/__pump_plugin_global_ctrl.h"

void Test_CB_WriteLog(
    PUMP_CORE_LOG_LEVEL emLogLevel,
    const char *szFileName,
    pump_uint32_t nLineNo,
    const char *szModularName,
    const char *szMessage,
    pump_uint32_t nMsgLen)
{
    switch (emLogLevel)
    {
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_ERROR:
        printf("[TestLog-E] %s\n", szMessage);
        break;
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_WARNING:
        printf("[TestLog-W] %s\n", szMessage);
        break;
    case PUMP_CORE_LOG_LEVEL::PUMP_LOG_INFO:
        printf("[TestLog-I] %s\n", szMessage);
        break;
    }
}

int main(int argc, char** argv)
{
    PUMP_CORE_Init();
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    struLogCong.pfnLog = Test_CB_WriteLog;
    pump_handle_t hLog = PUMP_CORE_LoggerCreate(PUMP_CORE_LOG_RECORED_USER);
    PUMP_CORE_LoggerConfig(hLog, &struLogCong);
    
    PUMP_PSLOT_Init();
    PUMP_PSLOT_CONF struConf = { 0 };
    strcpy(struConf.szInstallPath, "E:/VMware/YZ/github/Pump/modules/pump_plugin/test/test_plugin/build/");
    PUMP_PSLOT_Config(&struConf);
    PUMP_PSLOT_InjectLocalLogger(hLog);

    pump_plugin_id id = PUMP_PSLOT_PluginLoad("test_plugin", "0.9.1");
    if (id == -1)
    {
        PUMP_PLUGIN_ERROR("load plugin failed");
        return getchar();
    }
    PUMP_PLUGIN_INFO("load plugin succeed");

    PUMP_PSLOT_Cleanup();
    PUMP_CORE_Cleanup();

    return getchar();
}